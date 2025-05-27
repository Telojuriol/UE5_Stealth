// AISense_EnhancedSight.cpp

#include "AISense_EnhancedSight.h"              // Primero el header de la propia clase
#include "Perception/AIPerceptionComponent.h"   // Para UAIPerceptionComponent
#include "Perception/AIPerceptionSystem.h"      // Para UAIPerceptionSystem::GetCurrent
#include "Perception/AIPerceptionTypes.h"       // Para FAIStimulus, FPerceptionListener, FActorPerceptionInfo etc.
#include "Perception/AISenseConfig.h"           // Para GetSenseConfig()
#include "AISenseConfig_EnhancedSight.h"        // Para Cast a la config específica
#include "EngineUtils.h"                        // Para TActorIterator
#include "GameFramework/Pawn.h"                 // Para APawn
#include "GameFramework/Actor.h"                // Para AActor
#include "Engine/World.h"                       // Para UWorld, GetWorld()
#include "Kismet/KismetSystemLibrary.h"         // Para DrawDebugLine (opcional)
#include "Math/Vector.h"                        // Para FVector
#include "Math/Rotator.h"                       // Para FRotator
#include "Math/UnrealMathUtility.h"             // Para FMath, UE_PI, UE_BIG_NUMBER
#include "DrawDebugHelpers.h"                   // Para DrawDebugArc y otras formas de debug

// Constructor
UAISense_EnhancedSight::UAISense_EnhancedSight()
{
    UE_LOG(LogTemp, Warning, TEXT("EnhancedSight Constructor() called"));
    RequestImmediateUpdate();
    NotifyType = EAISenseNotifyType::OnEveryPerception;
}

// PostInitProperties
void UAISense_EnhancedSight::PostInitProperties()
{
    Super::PostInitProperties();
    UE_LOG(LogTemp, Warning, TEXT("UAISense_EnhancedSight::PostInitProperties() CALLED"));
}

// Update
float UAISense_EnhancedSight::Update()
{
    UE_LOG(LogTemp, Log, TEXT("UAISense_EnhancedSight::Update() --- LLAMADO ---"));
    ProcessSight();
    return 0.f;
}

// Implementación de CalculateFovApexOffset
float UAISense_EnhancedSight::CalculateFovApexOffset(
    float InitialFovDegrees,
    float FinalFovDegrees,
    float ThresholdRadialDistance) const // Renombrado para claridad: esta es la distancia al borde del primer FOV
{
    // Sanity checks
    if (ThresholdRadialDistance < KINDA_SMALL_NUMBER ||
        InitialFovDegrees < KINDA_SMALL_NUMBER || InitialFovDegrees >= 179.9f || // Evitar FOV de 0 o 180
        FinalFovDegrees < KINDA_SMALL_NUMBER || FinalFovDegrees >= 179.9f)     // Evitar FOV de 0 o 180
    {
        return 0.0f; // No offset para casos extremos o inválidos
    }

    const float HalfInitialFovRad = FMath::DegreesToRadians(InitialFovDegrees * 0.5f);
    const float HalfFinalFovRad = FMath::DegreesToRadians(FinalFovDegrees * 0.5f);

    // Si algún semiángulo es cercano a PI/2 (FOV cercano a 180), Tan se vuelve problemático.
    if (FMath::Abs(HalfInitialFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER ||
        FMath::Abs(HalfFinalFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER) {
        return 0.0f; // Sin offset si el FOV es prácticamente de 180 grados
    }

    // Calcular la geometría del primer cono de FOV (el inicial)
    // ThresholdRadialDistance es la hipotenusa/longitud del borde del sector inicial.
    // ActualLastRight: la mitad del ancho de la base del primer sector de FOV.
    // Similar a: lastRight = Mathf.Sin(Mathf.Deg2Rad * initial_fov / 2) * lastHipotenusa;
    float ActualLastRight = ThresholdRadialDistance * FMath::Sin(HalfInitialFovRad);

    // ActualLastForward: la distancia "hacia adelante" desde el vértice hasta el centro de la base del primer sector.
    // Similar a: lastForward = Mathf.Cos(Mathf.Deg2Rad * initial_fov / 2) * lastHipotenusa;
    float ActualLastForward = ThresholdRadialDistance * FMath::Cos(HalfInitialFovRad);

    // Ahora, calcular ForwardDiff para el FOV final basado en ActualLastRight.
    // ForwardDiff es la distancia "hacia adelante" que el vértice del FOV final necesitaría
    // para tener un ancho de ActualLastRight en su base.
    float ForwardDiff;
    float TanHalfFinal = FMath::Tan(HalfFinalFovRad);

    if (FMath::IsNearlyZero(TanHalfFinal)) // El FOV final es prácticamente 0 grados
    {
        // Si el ancho objetivo (ActualLastRight) también es 0 (porque el FOV inicial era 0),
        // entonces el vértice puede estar en el mismo plano, ForwardDiff = ActualLastForward, Offset = 0.
        // Si ActualLastRight > 0, un FOV de 0 no puede alcanzar este ancho a menos que el vértice esté infinitamente lejos.
        ForwardDiff = FMath::IsNearlyZero(ActualLastRight) ? ActualLastForward : UE_BIG_NUMBER;
    }
    else
    {
        ForwardDiff = ActualLastRight / TanHalfFinal;
    }

    // Controlar valores extremos de ForwardDiff para evitar overflow/underflow en el float del offset
    if (ForwardDiff > (UE_BIG_NUMBER / 2.0f))
    {
        ForwardDiff = (UE_BIG_NUMBER / 2.0f);
    }
    else if (ForwardDiff < (-UE_BIG_NUMBER / 2.0f))
    {
        ForwardDiff = (-UE_BIG_NUMBER / 2.0f);
    }

    // El offset es la diferencia entre dónde termina el "avance" del primer cono
    // y dónde necesitaría empezar el "avance" del segundo cono para compartir la misma base.
    return ActualLastForward - ForwardDiff;
}

// Implementación de IsInFovAndRange
bool UAISense_EnhancedSight::IsInFovAndRange(
    const FVector& ViewPoint,
    const FVector& ListenerHorizontalForwardNormalized,
    float MaxRangeSquared,
    const AActor* TargetActor,
    float ThresholdHorizontalDistanceSq,
    float InitialFovHorizontalDotProduct,
    float FinalFovHorizontalDotProduct,
    float CachedApexOffset,
    float& OutDistanceToTarget,
    FVector& OutDirectionToTargetNormalized
) const
{
    if (!TargetActor)
    {
        return false;
    }

    const FVector TargetLocation = TargetActor->GetActorLocation();
    FVector DirectionToTarget3D = TargetLocation - ViewPoint;
    const float DistanceToTargetSq3D = DirectionToTarget3D.SizeSquared();

    UE_LOG(LogTemp, Log, TEXT("DistanceToTargetSq3D %.2f MaxRangeSquared %.2f"), DistanceToTargetSq3D, MaxRangeSquared);

    if (DistanceToTargetSq3D > MaxRangeSquared)
    {
        OutDistanceToTarget = FMath::Sqrt(DistanceToTargetSq3D);
        OutDirectionToTargetNormalized = DirectionToTarget3D.GetSafeNormal();
        return false;
    }

    OutDistanceToTarget = FMath::Sqrt(DistanceToTargetSq3D);
    if (OutDistanceToTarget < KINDA_SMALL_NUMBER)
    {
        OutDirectionToTargetNormalized = ListenerHorizontalForwardNormalized;
        return true;
    }
    OutDirectionToTargetNormalized = DirectionToTarget3D / OutDistanceToTarget;

    FVector TargetLocationHorizontal = TargetLocation;
    TargetLocationHorizontal.Z = ViewPoint.Z;

    FVector DirectionToTargetHorizontal = TargetLocationHorizontal - ViewPoint;
    const float DistanceToTargetHorizontalSq = DirectionToTargetHorizontal.SizeSquared();
    FVector DirectionToTargetHorizontalNormalized = DirectionToTargetHorizontal.GetSafeNormal();

    if (DirectionToTargetHorizontal.IsNearlyZero())
    {
        return true;
    }

    float RelevantHorizontalDotProduct;
    FVector OriginForFovCheckHorizontal = ViewPoint;

    if (DistanceToTargetHorizontalSq <= ThresholdHorizontalDistanceSq)
    {
        RelevantHorizontalDotProduct = InitialFovHorizontalDotProduct;
    }
    else
    {
        RelevantHorizontalDotProduct = FinalFovHorizontalDotProduct;
        OriginForFovCheckHorizontal = ViewPoint + ListenerHorizontalForwardNormalized * CachedApexOffset;

        DirectionToTargetHorizontal = TargetLocationHorizontal - OriginForFovCheckHorizontal;
        DirectionToTargetHorizontalNormalized = DirectionToTargetHorizontal.GetSafeNormal();

        if (DirectionToTargetHorizontal.IsNearlyZero()) return true;
    }

    const float DotProductHorizontal = FVector::DotProduct(ListenerHorizontalForwardNormalized, DirectionToTargetHorizontalNormalized);

    return DotProductHorizontal >= RelevantHorizontalDotProduct;
}

// Implementación de HasLineOfSight
bool UAISense_EnhancedSight::HasLineOfSight(
    const UWorld* World,
    const FVector& ViewPoint,
    const AActor* TargetActor,
    const AActor* IgnoredActorForTrace
) const
{
    if (!World || !TargetActor) return false;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(IgnoredActorForTrace);

    const FVector TargetLocation = TargetActor->GetActorLocation();

    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        ViewPoint,
        TargetLocation,
        ECC_Visibility,
        QueryParams
    );

    if (!bHit)
    {
        return true;
    }
    else
    {
        return HitResult.GetActor() == TargetActor;
    }
}

static void DrawDebugArcManually(
    const UWorld* InWorld,
    const FVector& Center,
    float Radius,
    const FVector& Axis,
    const FVector& ForwardVector, // El vector central del arco (hacia donde "mira" el FOV)
    float HalfAngleDegrees,       // La mitad del ángulo total del arco (ej. FOV/2)
    int32 NumSegments,
    const FColor& Color,
    float LifeTime,
    float Thickness
)
{
    if (!InWorld || NumSegments <= 0 || Radius <= 0.f || HalfAngleDegrees <= 0.f)
    {
        return;
    }

    const float HalfAngleRad = FMath::DegreesToRadians(HalfAngleDegrees);
    const float AngleStep = (HalfAngleRad * 2.0f) / FMath::Max(1, NumSegments); // Ángulo de cada segmento

    FVector LastVertex = Center + ForwardVector.RotateAngleAxisRad(-HalfAngleRad, Axis) * Radius;

    for (int32 i = 1; i <= NumSegments; ++i)
    {
        float CurrentRelativeAngleRad = -HalfAngleRad + (static_cast<float>(i) * AngleStep);
        FVector CurrentVertex = Center + ForwardVector.RotateAngleAxisRad(CurrentRelativeAngleRad, Axis) * Radius;
        DrawDebugLine(InWorld, LastVertex, CurrentVertex, Color, false, LifeTime, 0, Thickness);
        LastVertex = CurrentVertex;
    }
}


void UAISense_EnhancedSight::ProcessSight()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const bool bDrawDebugFov = true; // Control para el debug visual

    AIPerception::FListenerMap* Listeners = GetListeners();
    if (!Listeners || Listeners->Num() == 0)
    {
        return;
    }

    for (AIPerception::FListenerMap::TIterator ListenerIt(*Listeners); ListenerIt; ++ListenerIt)
    {
        FPerceptionListener& Listener = ListenerIt->Value;
        const AActor* ListenerActor = Listener.GetBodyActor();
        UAIPerceptionComponent* PerceptionComponent = Listener.Listener.Get();

        if (!ListenerActor || !PerceptionComponent)
        {
            continue;
        }

        const UAISenseConfig_EnhancedSight* MyConfig = Cast<const UAISenseConfig_EnhancedSight>(PerceptionComponent->GetSenseConfig(GetSenseID()));

        if (!MyConfig)
        {
            continue;
        }

        // Parámetros de Configuración del FOV
        const float InitialFovDegrees = MyConfig->PeripheralVisionAngle;
        const float FinalFovDegrees = MyConfig->FinalPeripheralVisionAngle;
        const float ThresholdHorizontalDist = MyConfig->FinalPeripheralVisionAngleThreesholdDistance; // Typo en UPROPERTY, mantenido por consistencia
        const float SightRadiusFromConfig = MyConfig->SightRadius;
        const float LoseSightRadiusFromConfig = MyConfig->LoseSightRadius;

        // Pre-cálculos para los dot products y offset (una vez por listener/config)
        const float ThresholdHorizontalDistSq = FMath::Square(ThresholdHorizontalDist);
        const float InitialFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(InitialFovDegrees * 0.5f));
        const float FinalFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(FinalFovDegrees * 0.5f));

        const float CachedApexOffset = CalculateFovApexOffset(
            InitialFovDegrees,
            FinalFovDegrees,
            ThresholdHorizontalDist
        );

        // Obtener ViewPoint y ListenerForward del Listener
        FVector ViewPoint;
        FVector ListenerForward3D;
        APawn* ListenerPawn = Cast<APawn>(const_cast<AActor*>(ListenerActor));
        if (ListenerPawn)
        {
            FRotator ViewRotation;
            ListenerPawn->GetActorEyesViewPoint(ViewPoint, ViewRotation);
            ListenerForward3D = ViewRotation.Vector();
        }
        else
        {
            ViewPoint = ListenerActor->GetActorLocation();
            ListenerForward3D = ListenerActor->GetActorForwardVector();
        }

        FVector ListenerHorizontalForward = ListenerForward3D;
        ListenerHorizontalForward.Z = 0.0f;
        if (!ListenerHorizontalForward.Normalize())
        {
            ListenerHorizontalForward = ListenerActor->GetActorForwardVector(); // Intenta con el del actor
            ListenerHorizontalForward.Z = 0.0f;
            if (!ListenerHorizontalForward.Normalize()) {
                ListenerHorizontalForward = FVector::ForwardVector; // Fallback absoluto
            }
        }

        // --- INICIO DEBUG VISUAL FOV (CORREGIDO PARA ARCOS EXTERIORES) ---
        if (bDrawDebugFov && World)
        {
            const FColor InitialFovColor = FColor::Green;
            const FColor LoseSightRadiusColor = FColor::Orange;
            // Usaremos InitialFovColor para el SightRadius para mantener la consistencia si es el único límite exterior.
            const FColor SightRadiusDisplayColor = InitialFovColor;

            const float DebugLifeTime = 0.0f;
            const float Thickness = 1.0f;
            const int32 ArcSegments = 24;
            const FVector RotationAxis = FVector::UpVector;

            // 1. Dibujar FOV Inicial hasta el umbral (SIN CAMBIOS)
            float HalfInitialFovDeg = InitialFovDegrees * 0.5f;
            FVector LeftEdgeDirInitial = ListenerHorizontalForward.RotateAngleAxis(-HalfInitialFovDeg, RotationAxis);
            FVector RightEdgeDirInitial = ListenerHorizontalForward.RotateAngleAxis(HalfInitialFovDeg, RotationAxis);
            FVector LeftEdgeEndInitial_AtThreshold = ViewPoint + LeftEdgeDirInitial * ThresholdHorizontalDist;
            FVector RightEdgeEndInitial_AtThreshold = ViewPoint + RightEdgeDirInitial * ThresholdHorizontalDist;

            DrawDebugLine(World, ViewPoint, LeftEdgeEndInitial_AtThreshold, InitialFovColor, false, DebugLifeTime, 0, Thickness);
            DrawDebugLine(World, ViewPoint, RightEdgeEndInitial_AtThreshold, InitialFovColor, false, DebugLifeTime, 0, Thickness);
            DrawDebugArcManually(World, ViewPoint, ThresholdHorizontalDist, RotationAxis, ListenerHorizontalForward, HalfInitialFovDeg, ArcSegments, InitialFovColor, DebugLifeTime, Thickness);

            // 2. Dibujo del FOV Extendido
            // VirtualOriginFinalFov sigue siendo necesario para la lógica de IsInFovAndRange y para entender la forma del cono final.
            FVector VirtualOriginFinalFov = ViewPoint + ListenerHorizontalForward * CachedApexOffset;
            float HalfFinalFovDeg = FinalFovDegrees * 0.5f; // Ángulo para la forma del FOV final

            // Puntos finales para el límite de SightRadius, PERO AHORA CALCULADOS DESDE ViewPoint
            FVector LeftEdgeEnd_AtSightRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(-HalfFinalFovDeg, RotationAxis) * SightRadiusFromConfig;
            FVector RightEdgeEnd_AtSightRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(HalfFinalFovDeg, RotationAxis) * SightRadiusFromConfig;

            // Dibujar líneas conectando el umbral con el límite de SightRadius
            DrawDebugLine(World, LeftEdgeEndInitial_AtThreshold, LeftEdgeEnd_AtSightRadius_FromViewPoint, SightRadiusDisplayColor, false, DebugLifeTime, 0, Thickness);
            DrawDebugLine(World, RightEdgeEndInitial_AtThreshold, RightEdgeEnd_AtSightRadius_FromViewPoint, SightRadiusDisplayColor, false, DebugLifeTime, 0, Thickness);

            // Dibujar Arco para SightRadius, CENTRADO EN ViewPoint, usando el ángulo del FOV final
            DrawDebugArcManually(World, ViewPoint, SightRadiusFromConfig, RotationAxis, ListenerHorizontalForward, HalfFinalFovDeg, ArcSegments, SightRadiusDisplayColor, DebugLifeTime, Thickness);

            // Si LoseSightRadius es mayor, dibuja su límite también, CENTRADO EN ViewPoint
            if (LoseSightRadiusFromConfig > SightRadiusFromConfig)
            {
                // Puntos finales para el límite de LoseSightRadius, CALCULADOS DESDE ViewPoint
                FVector LeftEdgeEnd_AtLoseRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(-HalfFinalFovDeg, RotationAxis) * LoseSightRadiusFromConfig;
                FVector RightEdgeEnd_AtLoseRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(HalfFinalFovDeg, RotationAxis) * LoseSightRadiusFromConfig;

                // Líneas conectando el límite de SightRadius con el límite de LoseSightRadius
                DrawDebugLine(World, LeftEdgeEnd_AtSightRadius_FromViewPoint, LeftEdgeEnd_AtLoseRadius_FromViewPoint, LoseSightRadiusColor, false, DebugLifeTime, 0, Thickness * 0.7f);
                DrawDebugLine(World, RightEdgeEnd_AtSightRadius_FromViewPoint, RightEdgeEnd_AtLoseRadius_FromViewPoint, LoseSightRadiusColor, false, DebugLifeTime, 0, Thickness * 0.7f);

                // Arco para LoseSightRadius, CENTRADO EN ViewPoint, usando el ángulo del FOV final
                DrawDebugArcManually(World, ViewPoint, LoseSightRadiusFromConfig, RotationAxis, ListenerHorizontalForward, HalfFinalFovDeg, ArcSegments, LoseSightRadiusColor, DebugLifeTime, Thickness * 0.7f);
            }

            // Dibujar el VirtualOriginFinalFov sigue siendo útil para entender la geometría de la *comprobación angular*
            if (FMath::Abs(CachedApexOffset) > KINDA_SMALL_NUMBER)
            {
                DrawDebugPoint(World, VirtualOriginFinalFov, 15.f, FColor::Magenta, false, DebugLifeTime);
                // Opcional: Dibuja las líneas del cono "real" desde el vértice virtual con un color tenue para entender la forma
                FVector TrueConeLeftEdgeDir = ListenerHorizontalForward.RotateAngleAxis(-HalfFinalFovDeg, RotationAxis);
                FVector TrueConeRightEdgeDir = ListenerHorizontalForward.RotateAngleAxis(HalfFinalFovDeg, RotationAxis);
                // DrawDebugLine(World, VirtualOriginFinalFov, VirtualOriginFinalFov + TrueConeLeftEdgeDir * SightRadiusFromConfig, FColor(255,0,255,64), false, DebugLifeTime,0,0.5f);
                // DrawDebugLine(World, VirtualOriginFinalFov, VirtualOriginFinalFov + TrueConeRightEdgeDir * SightRadiusFromConfig, FColor(255,0,255,64), false, DebugLifeTime,0,0.5f);
            }
        }
        

        for (TActorIterator<APawn> TargetIt(World); TargetIt; ++TargetIt)
        {
            APawn* TargetActorAsPawn = *TargetIt;

            if (!TargetActorAsPawn || TargetActorAsPawn == ListenerActor)
            {
                continue;
            }

            AActor* TargetActor = TargetActorAsPawn;

            // --- Determinar si el objetivo ya era visible ---
            const FActorPerceptionInfo* TargetPerceptionInfo = PerceptionComponent->GetActorInfo(*TargetActor);
            bool bWasSuccessfullySensedPreviously = false;
            if (TargetPerceptionInfo)
            {
                int32 SenseIndex = GetSenseID().Index;
                if (TargetPerceptionInfo->LastSensedStimuli.IsValidIndex(SenseIndex))
                {
                    bWasSuccessfullySensedPreviously = TargetPerceptionInfo->LastSensedStimuli[SenseIndex].WasSuccessfullySensed();
                }
            }

            // --- Determinar el radio máximo a usar para LA COMPROBACIÓN ACTUAL ---
            float RangeToCheckThisFrame;
            if (bWasSuccessfullySensedPreviously)
            {
                RangeToCheckThisFrame = FMath::Max(SightRadiusFromConfig, LoseSightRadiusFromConfig);
            }
            else
            {
                RangeToCheckThisFrame = SightRadiusFromConfig;
            }
            const float MaxRangeSqForCheck = FMath::Square(RangeToCheckThisFrame);

            // Parámetros de salida para IsInFovAndRange
            float DistanceToTarget;
            FVector DirectionToTarget;
            const FVector TargetLocation = TargetActor->GetActorLocation();
            FVector DirectionToTarget3D = TargetLocation - ViewPoint;
            const float DistanceToTargetSq3D = DirectionToTarget3D.Size();
            UE_LOG(LogTemp, Log, TEXT("DistanceToTargetSq3D %.2f MaxRangeSquared %.2f"), DistanceToTargetSq3D, RangeToCheckThisFrame);
            bool bIsInFovAndRange = IsInFovAndRange(
                ViewPoint,
                ListenerHorizontalForward,
                MaxRangeSqForCheck, // <--- USA EL RADIO CALCULADO DINÁMICAMENTE
                TargetActor,
                ThresholdHorizontalDistSq,
                InitialFovHorizontalDot,
                FinalFovHorizontalDot,
                CachedApexOffset,
                DistanceToTarget,
                DirectionToTarget
            );

            bool bCurrentlyHasLineOfSight = false;
            if (bIsInFovAndRange)
            {
                bCurrentlyHasLineOfSight = HasLineOfSight(World, ViewPoint, TargetActor, ListenerActor);
            }
            const bool bIsCurrentlyVisible = bIsInFovAndRange && bCurrentlyHasLineOfSight;

            FAIStimulus::FResult CurrentSenseResult = bIsCurrentlyVisible ? FAIStimulus::SensingSucceeded : FAIStimulus::SensingFailed;
            bool bShouldReportStimulus = false;
            
            if (bIsCurrentlyVisible)
            {
                bShouldReportStimulus = true;
                if (!bWasSuccessfullySensedPreviously) {
                    UE_LOG(LogTemp, Log, TEXT("IA (%s) HA VISTO A (%s). Dist: %.2f"), *ListenerActor->GetName(), *TargetActor->GetName(), DistanceToTarget);
                }
            }
            else
            {
                if (bWasSuccessfullySensedPreviously)
                {
                    bShouldReportStimulus = true;
                    UE_LOG(LogTemp, Log, TEXT("IA (%s) HA PERDIDO DE VISTA A (%s)."), *ListenerActor->GetName(), *TargetActor->GetName());
                }
            }

            if (bShouldReportStimulus)
            {
                const FVector ActualTargetLocation = TargetActor->GetActorLocation();
                FAIStimulus Stimulus(
                    *this,
                    1.0f,
                    ActualTargetLocation,
                    ViewPoint,
                    CurrentSenseResult,
                    FName()
                );
                if (TargetActor)
                {
                    Listener.RegisterStimulus(TargetActor, Stimulus);
                }
            }
        }
    }
}