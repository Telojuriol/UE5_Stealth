// AISense_EnhancedSight.cpp

#include "AISense_EnhancedSight.h"         // Primero el header de la propia clase
#include "Perception/AIPerceptionComponent.h" // Para UAIPerceptionComponent
#include "Perception/AIPerceptionSystem.h"  // Para UAIPerceptionSystem::GetCurrent
#include "Perception/AIPerceptionTypes.h"   // Para FAIStimulus, FPerceptionListener, FActorPerceptionInfo etc.
#include "Perception/AISenseConfig.h"       // Para GetSenseConfig()
#include "AISenseConfig_EnhancedSight.h"    // Para Cast a la config específica
#include "EngineUtils.h"                    // Para TActorIterator
#include "GameFramework/Pawn.h"             // Para APawn
#include "GameFramework/Actor.h"            // Para AActor
#include "Engine/World.h"                   // Para UWorld, GetWorld()
#include "Kismet/KismetSystemLibrary.h"     // Para DrawDebugLine (opcional)
#include "Math/Vector.h"                    // Para FVector
#include "Math/Rotator.h"                   // Para FRotator
#include "Math/UnrealMathUtility.h"         // Para FMath, UE_PI, UE_BIG_NUMBER

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
    return 0.5f;
}

// Implementación de CalculateFovApexOffset
float UAISense_EnhancedSight::CalculateFovApexOffset(
    float InitialFovDegrees,
    float FinalFovDegrees,
    float ThresholdHorizontalDistance) const
{
    if (ThresholdHorizontalDistance < KINDA_SMALL_NUMBER ||
        InitialFovDegrees < KINDA_SMALL_NUMBER || InitialFovDegrees >= 179.9f ||
        FinalFovDegrees < KINDA_SMALL_NUMBER || FinalFovDegrees >= 179.9f)
    {
        return 0.0f;
    }

    const float HalfInitialFovRad = FMath::DegreesToRadians(InitialFovDegrees * 0.5f);
    const float HalfFinalFovRad = FMath::DegreesToRadians(FinalFovDegrees * 0.5f);

    if (FMath::Abs(HalfInitialFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER || FMath::Abs(HalfFinalFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER) {
        return 0.0f;
    }

    float TanHalfInitial = FMath::Tan(HalfInitialFovRad);
    float TanHalfFinal = FMath::Tan(HalfFinalFovRad);

    float LastRight;
    if (FMath::IsNearlyZero(TanHalfInitial))
        LastRight = 0.0f;
    else
        LastRight = ThresholdHorizontalDistance * TanHalfInitial;

    const float LastForward = ThresholdHorizontalDistance;

    float ForwardDiff;
    if (FMath::IsNearlyZero(TanHalfFinal))
    {
        ForwardDiff = FMath::IsNearlyZero(LastRight) ? LastForward : UE_BIG_NUMBER;
    }
    else
    {
        ForwardDiff = LastRight / TanHalfFinal;
    }

    if (ForwardDiff > (UE_BIG_NUMBER / 2.0f))
    {
        return LastForward - (UE_BIG_NUMBER / 2.0f);
    }
    if (ForwardDiff < (-UE_BIG_NUMBER / 2.0f))
    {
        return LastForward - (-UE_BIG_NUMBER / 2.0f);
    }

    return LastForward - ForwardDiff;
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

// Implementación de ProcessSight
void UAISense_EnhancedSight::ProcessSight()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

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

        const UAISenseConfig* FoundConfig = PerceptionComponent->GetSenseConfig(GetSenseID());
        const UAISenseConfig_EnhancedSight* MyConfig = Cast<const UAISenseConfig_EnhancedSight>(FoundConfig);

        if (!MyConfig)
        {
            continue;
        }

        const float MaxRangeFromConfig = MyConfig->SightRadius;
        const float MaxRangeSq = FMath::Square(MaxRangeFromConfig);

        const float ThresholdHorizontalDist = MyConfig->FinalPeripheralVisionAngleThreesholdDistance;
        const float ThresholdHorizontalDistSq = FMath::Square(ThresholdHorizontalDist);

        const float InitialFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(MyConfig->PeripheralVisionAngle * 0.5f));
        const float FinalFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(MyConfig->FinalPeripheralVisionAngle * 0.5f));

        const float CachedApexOffset = CalculateFovApexOffset(
            MyConfig->PeripheralVisionAngle,
            MyConfig->FinalPeripheralVisionAngle,
            ThresholdHorizontalDist
        );

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
            ListenerHorizontalForward = ListenerActor->GetActorForwardVector();
            ListenerHorizontalForward.Z = 0.0f;
            if (!ListenerHorizontalForward.Normalize()) {
                ListenerHorizontalForward = FVector::ForwardVector;
            }
        }

        for (TActorIterator<APawn> TargetIt(World); TargetIt; ++TargetIt)
        {
            APawn* TargetActorAsPawn = *TargetIt; // Renombrado para claridad

            if (!TargetActorAsPawn || TargetActorAsPawn == ListenerActor) // Comparar con ListenerActor
            {
                continue;
            }

            AActor* TargetActor = TargetActorAsPawn; // Upcast APawn* to AActor* para IsInFovAndRange y otros usos

            float DistanceToTarget;
            FVector DirectionToTarget;

            bool bIsInFovAndRange = IsInFovAndRange(
                ViewPoint,
                ListenerHorizontalForward,
                MaxRangeSq,
                TargetActor, // Pasar AActor*
                ThresholdHorizontalDistSq,
                InitialFovHorizontalDot,
                FinalFovHorizontalDot,
                CachedApexOffset,
                DistanceToTarget,
                DirectionToTarget
            );
            UE_LOG(LogTemp, Log, TEXT("bIsInFovAndRange: %s"), bIsInFovAndRange ? TEXT("true") : TEXT("false"));
            bool bCurrentlyHasLineOfSight = false;
            if (bIsInFovAndRange)
            {
                bCurrentlyHasLineOfSight = HasLineOfSight(World, ViewPoint, TargetActor, ListenerActor);
            }
            const bool bIsCurrentlyVisible = bIsInFovAndRange && bCurrentlyHasLineOfSight;

            const FActorPerceptionInfo* TargetPerceptionInfo = PerceptionComponent->GetActorInfo(*TargetActor);
            bool bWasSuccessfullySensedPreviously = false;
            if (TargetPerceptionInfo)
            {
                // CORRECCIÓN: Usar GetSenseID().GetIDValue()
                int32 SenseIndex = GetSenseID().Index;
                if (TargetPerceptionInfo->LastSensedStimuli.IsValidIndex(SenseIndex))
                {
                    bWasSuccessfullySensedPreviously = TargetPerceptionInfo->LastSensedStimuli[SenseIndex].WasSuccessfullySensed();
                }
            }

            // CORRECCIÓN: Usar FAIStimulus::FResult según tu indicación
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
                // CORRECCIÓN: Asegurarse de que TargetActor (como AActor*) se dereferencia correctamente para AActor&
                // y que no haya ambigüedad. La variable TargetActor ya es AActor* aquí.
                // El APawn* original es TargetActorAsPawn.
                if (TargetActor)
                {
                    Listener.RegisterStimulus(TargetActor, Stimulus); // TargetActor es AActor*, *TargetActor es AActor&
                }
            }
        }
    }
}