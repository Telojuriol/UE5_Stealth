// AISense_EnhancedSight.cpp

#include "AISense_EnhancedSight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig.h"
#include "AISenseConfig_EnhancedSight.h" // Your specific config class
#include "EngineUtils.h"                 // For TActorIterator
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h" // For ACharacter
#include "Components/CapsuleComponent.h" // For UCapsuleComponent
#include "Components/SkeletalMeshComponent.h" // <<<<<<< AÑADIR ESTE INCLUDE
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h" // For DrawDebugPoint
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/UnrealMathUtility.h"     // For FMath, UE_PI, UE_BIG_NUMBER
#include "DrawDebugHelpers.h"           // For DrawDebugLine

// ... (DrawDebugArcManuallyCapped and DrawDebugArcManuallySimple remain unchanged) ...
// File-static helper function for drawing debug arcs with Z-capping.
static void DrawDebugArcManuallyCapped(
    const UWorld* InWorld,
    const FVector& Center,      // ViewPoint
    float Radius,               // Intended radial extent of the arc
    const FVector& Axis,          // Axis of rotation for the arc (e.g., ListenerRight3D)
    const FVector& ForwardVector, // Central direction of the arc (e.g., ListenerActualForward3D)
    float HalfAngleDegrees,   // Half of the total arc angle (e.g., VerticalFOV/2)
    int32 NumSegments,
    const FColor& Color,
    float LifeTime,
    float Thickness,
    float MaxZOffsetUp,       // MaxDistUp from ViewPoint's Z
    float MaxZOffsetDown      // MaxDistDown from ViewPoint's Z (positive value)
)
{
    if (!InWorld || NumSegments <= 0 || Radius < KINDA_SMALL_NUMBER || HalfAngleDegrees < KINDA_SMALL_NUMBER)
    {
        return;
    }

    auto CapVertexZ = [&](FVector VertexToCap) -> FVector
        {
            float VertexZRelativeToCenter = VertexToCap.Z - Center.Z;
            if (VertexZRelativeToCenter > MaxZOffsetUp)
            {
                VertexToCap.Z = Center.Z + MaxZOffsetUp;
            }
            else if (VertexZRelativeToCenter < -MaxZOffsetDown)
            {
                VertexToCap.Z = Center.Z - MaxZOffsetDown;
            }
            return VertexToCap;
        };

    const float HalfAngleRad = FMath::DegreesToRadians(HalfAngleDegrees);
    const float AngleStep = (NumSegments > 0 && !FMath::IsNearlyZero(HalfAngleRad * 2.0f))
        ? ((HalfAngleRad * 2.0f) / static_cast<float>(NumSegments))
        : 0.0f;

    FVector StartDirection = ForwardVector.RotateAngleAxisRad(-HalfAngleRad, Axis);
    FVector LastVertexOnSphere = Center + StartDirection * Radius;
    FVector LastCappedVertex = CapVertexZ(LastVertexOnSphere);

    if (FMath::IsNearlyZero(AngleStep) && NumSegments > 0)
    {
        FVector EndDirection = ForwardVector.RotateAngleAxisRad(HalfAngleRad, Axis);
        FVector EndVertexOnSphere = Center + EndDirection * Radius;
        FVector EndCappedVertex = CapVertexZ(EndVertexOnSphere);
        // For a very small angle, draw a representation of the small capped sector
        DrawDebugLine(InWorld, CapVertexZ(Center), LastCappedVertex, Color, false, LifeTime, 0, Thickness);
        DrawDebugLine(InWorld, CapVertexZ(Center), EndCappedVertex, Color, false, LifeTime, 0, Thickness);
        DrawDebugLine(InWorld, LastCappedVertex, EndCappedVertex, Color, false, LifeTime, 0, Thickness);
        return;
    }

    for (int32 i = 1; i <= NumSegments; ++i)
    {
        float CurrentRelativeAngleRad = -HalfAngleRad + (static_cast<float>(i) * AngleStep);
        FVector CurrentDirection = ForwardVector.RotateAngleAxisRad(CurrentRelativeAngleRad, Axis);
        FVector CurrentVertexOnSphere = Center + CurrentDirection * Radius;
        FVector CurrentCappedVertex = CapVertexZ(CurrentVertexOnSphere);

        DrawDebugLine(InWorld, LastCappedVertex, CurrentCappedVertex, Color, false, LifeTime, 0, Thickness);
        LastCappedVertex = CurrentCappedVertex;
    }
}

// File-static helper for standard (non-capped) arcs - for horizontal FOV
static void DrawDebugArcManuallySimple(
    const UWorld* InWorld, const FVector& Center, float Radius, const FVector& Axis,
    const FVector& ForwardVector, float HalfAngleDegrees, int32 NumSegments,
    const FColor& Color, float LifeTime, float Thickness)
{
    if (!InWorld || NumSegments <= 0 || Radius < KINDA_SMALL_NUMBER || HalfAngleDegrees < KINDA_SMALL_NUMBER) return;
    const float HalfAngleRad = FMath::DegreesToRadians(HalfAngleDegrees);
    const float AngleStep = (NumSegments > 0 && !FMath::IsNearlyZero(HalfAngleRad * 2.0f))
        ? ((HalfAngleRad * 2.0f) / static_cast<float>(NumSegments)) : 0.0f;
    FVector StartDirection = ForwardVector.RotateAngleAxisRad(-HalfAngleRad, Axis);
    FVector LastVertex = Center + StartDirection * Radius;
    if (FMath::IsNearlyZero(AngleStep) && NumSegments > 0)
    {
        FVector EndDirection = ForwardVector.RotateAngleAxisRad(HalfAngleRad, Axis);
        FVector EndVertex = Center + EndDirection * Radius;
        DrawDebugLine(InWorld, Center, LastVertex, Color, false, LifeTime, 0, Thickness);
        DrawDebugLine(InWorld, Center, EndVertex, Color, false, LifeTime, 0, Thickness);
        DrawDebugLine(InWorld, LastVertex, EndVertex, Color, false, LifeTime, 0, Thickness);
        return;
    }
    for (int32 i = 1; i <= NumSegments; ++i)
    {
        float CurrentRelativeAngleRad = -HalfAngleRad + (static_cast<float>(i) * AngleStep);
        FVector CurrentDirection = ForwardVector.RotateAngleAxisRad(CurrentRelativeAngleRad, Axis);
        FVector CurrentVertex = Center + CurrentDirection * Radius;
        DrawDebugLine(InWorld, LastVertex, CurrentVertex, Color, false, LifeTime, 0, Thickness);
        LastVertex = CurrentVertex;
    }
}


UAISense_EnhancedSight::UAISense_EnhancedSight()
{
    UE_LOG(LogTemp, Warning, TEXT("EnhancedSight Constructor() called"));
    RequestImmediateUpdate();
    NotifyType = EAISenseNotifyType::OnEveryPerception;
}

void UAISense_EnhancedSight::PostInitProperties()
{
    Super::PostInitProperties();
    UE_LOG(LogTemp, Warning, TEXT("UAISense_EnhancedSight::PostInitProperties() CALLED"));
}

float UAISense_EnhancedSight::Update()
{
    ProcessSight();
    return 0.f;
}

float UAISense_EnhancedSight::CalculateFovApexOffset(
    float InitialFovDegrees,
    float FinalFovDegrees,
    float ThresholdRadialDistance) const
{
    if (ThresholdRadialDistance < KINDA_SMALL_NUMBER ||
        InitialFovDegrees < KINDA_SMALL_NUMBER || InitialFovDegrees >= 179.9f ||
        FinalFovDegrees < KINDA_SMALL_NUMBER || FinalFovDegrees >= 179.9f)
    {
        return 0.0f;
    }
    const float HalfInitialFovRad = FMath::DegreesToRadians(InitialFovDegrees * 0.5f);
    const float HalfFinalFovRad = FMath::DegreesToRadians(FinalFovDegrees * 0.5f);

    if (FMath::Abs(HalfInitialFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER ||
        FMath::Abs(HalfFinalFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER) {
        return 0.0f;
    }

    float ActualLastRight = ThresholdRadialDistance * FMath::Sin(HalfInitialFovRad);
    float ActualLastForward = ThresholdRadialDistance * FMath::Cos(HalfInitialFovRad);

    float ForwardDiff;
    float TanHalfFinal = FMath::Tan(HalfFinalFovRad);
    if (FMath::IsNearlyZero(TanHalfFinal))
    {
        ForwardDiff = FMath::IsNearlyZero(ActualLastRight) ? ActualLastForward : UE_BIG_NUMBER;
    }
    else
    {
        ForwardDiff = ActualLastRight / TanHalfFinal;
    }

    if (ForwardDiff > (UE_BIG_NUMBER / 2.0f)) ForwardDiff = (UE_BIG_NUMBER / 2.0f);
    else if (ForwardDiff < (-UE_BIG_NUMBER / 2.0f)) ForwardDiff = (-UE_BIG_NUMBER / 2.0f);

    return ActualLastForward - ForwardDiff;
}

bool UAISense_EnhancedSight::IsInFovAndRange(
    const FVector& ViewPoint,
    const FVector& ListenerHorizontalForwardNormalized,
    const FVector& ListenerActualForward3D,
    const UAISenseConfig_EnhancedSight* SenseConfig,
    float MaxHorizontalRangeSquared,
    const AActor* TargetActor,
    float ThresholdHorizontalDistanceSq,
    float InitialFovHorizontalDotProduct,
    float FinalFovHorizontalDotProduct,
    float CachedApexOffset,
    float& OutDistanceToTarget3D,
    FVector& OutDirectionToTarget3DNormalized
) const
{
    if (!TargetActor || !SenseConfig)
    {
        return false;
    }

    const FVector TargetLocation = TargetActor->GetActorLocation();
    FVector DirectionToTarget3D = TargetLocation - ViewPoint;
    const float DistanceToTargetSq3D = DirectionToTarget3D.SizeSquared();

    OutDistanceToTarget3D = FMath::Sqrt(DistanceToTargetSq3D);
    if (OutDistanceToTarget3D < KINDA_SMALL_NUMBER)
    {
        OutDirectionToTarget3DNormalized = ListenerActualForward3D;
        return true;
    }
    OutDirectionToTarget3DNormalized = DirectionToTarget3D / OutDistanceToTarget3D;

    // --- Horizontal Range Check ---
    FVector TargetLocationHorizontal = TargetLocation;
    TargetLocationHorizontal.Z = ViewPoint.Z;
    FVector DirectionToTargetHorizontal = TargetLocationHorizontal - ViewPoint;
    const float DistanceToTargetHorizontalSq = DirectionToTargetHorizontal.SizeSquared();

    if (DistanceToTargetHorizontalSq > MaxHorizontalRangeSquared)
    {
        return false;
    }

    // --- Horizontal FOV Angle Check ---
    FVector DirectionToTargetHorizontalNormalized = DirectionToTargetHorizontal.GetSafeNormal();
    bool bIsInHorizontalAngle = true;
    if (!DirectionToTargetHorizontal.IsNearlyZero())
    {
        float RelevantHorizontalDotProduct;
        FVector OriginForAngleCheckHorizontal = ViewPoint;

        if (DistanceToTargetHorizontalSq <= ThresholdHorizontalDistanceSq)
        {
            RelevantHorizontalDotProduct = InitialFovHorizontalDotProduct;
        }
        else
        {
            RelevantHorizontalDotProduct = FinalFovHorizontalDotProduct;
            OriginForAngleCheckHorizontal = ViewPoint + ListenerHorizontalForwardNormalized * CachedApexOffset;
            DirectionToTargetHorizontal = TargetLocationHorizontal - OriginForAngleCheckHorizontal;
            DirectionToTargetHorizontalNormalized = DirectionToTargetHorizontal.GetSafeNormal();
        }

        if (!DirectionToTargetHorizontal.IsNearlyZero())
        {
            const float DotProductHorizontal = FVector::DotProduct(ListenerHorizontalForwardNormalized, DirectionToTargetHorizontalNormalized);
            bIsInHorizontalAngle = DotProductHorizontal >= RelevantHorizontalDotProduct;
        }
    }

    if (!bIsInHorizontalAngle)
    {
        return false;
    }

    // --- Vertical Distance Limits Check (MaxDistUp / MaxDistDown) ---
    const float VerticalOffsetToTarget = TargetLocation.Z - ViewPoint.Z;
    if (VerticalOffsetToTarget > SenseConfig->MaxDistUp || VerticalOffsetToTarget < -SenseConfig->MaxDistDown)
    {
        return false;
    }

    // --- Vertical FOV Angle Check ---
    const float DotProductVertical = FVector::DotProduct(ListenerActualForward3D, OutDirectionToTarget3DNormalized);
    const float RequiredDotVertical = FMath::Cos(FMath::DegreesToRadians(SenseConfig->VerticalPeripheralVisionAngle * 0.5f));

    if (DotProductVertical < RequiredDotVertical)
    {
        return false;
    }

    return true; // Passed all checks
}

// MODIFIED FUNCTION //
TArray<FVector> UAISense_EnhancedSight::GetRaycastTargetPoints(const AActor* TargetActor) const
{
    TArray<FVector> TargetPoints;
    if (!TargetActor) return TargetPoints;

    const ACharacter* TargetCharacter = Cast<const ACharacter>(TargetActor);
    USkeletalMeshComponent* Mesh = TargetCharacter ? TargetCharacter->GetMesh() : nullptr;

    // Nombres de huesos (pueden necesitar ser configurables o tener alternativas para diferentes esqueletos)
    // Para el esqueleto de Mannequin de UE: "head", "pelvis", "spine_03" (pecho alto/columna superior)
    const FName HeadBoneName = FName("head");
    const FName PelvisBoneName = FName("pelvis");
    const FName UpperChestBoneName = FName("spine_05"); // Hueso común para la parte superior del pecho/columna
    const FName AltUpperChestBoneName1 = FName("spine_04"); // Alternativa si spine_03 no existe
    const FName AltUpperChestBoneName2 = FName("clavicle_r"); // Otra alternativa (clavícula derecha)


    bool bUsedBones = false;

    if (Mesh && Mesh->SkeletalMesh && Mesh->GetNumBones() > 0)
    {
        FVector HeadLocation, PelvisLocation, UpperChestLocation;
        bool bGotHead = false;
        bool bGotPelvis = false;
        bool bGotUpperChest = false;

        // Obtener localización de la cabeza
        if (Mesh->GetBoneIndex(HeadBoneName) != INDEX_NONE)
        {
            HeadLocation = Mesh->GetSocketLocation(HeadBoneName);
            bGotHead = true;
        }

        // Obtener localización de la pelvis
        if (Mesh->GetBoneIndex(PelvisBoneName) != INDEX_NONE)
        {
            PelvisLocation = Mesh->GetSocketLocation(PelvisBoneName);
            bGotPelvis = true;
        }

        // Obtener localización del pecho alto
        if (Mesh->GetBoneIndex(UpperChestBoneName) != INDEX_NONE)
        {
            UpperChestLocation = Mesh->GetSocketLocation(UpperChestBoneName);
            bGotUpperChest = true;
        }
        else if (Mesh->GetBoneIndex(AltUpperChestBoneName1) != INDEX_NONE) // Fallback 1 para pecho alto
        {
            UpperChestLocation = Mesh->GetSocketLocation(AltUpperChestBoneName1);
            bGotUpperChest = true;
        }
        else if (Mesh->GetBoneIndex(AltUpperChestBoneName2) != INDEX_NONE) // Fallback 2 para pecho alto
        {
            UpperChestLocation = Mesh->GetSocketLocation(AltUpperChestBoneName2);
            bGotUpperChest = true;
        }

        // Añadir puntos en el orden solicitado: Cabeza, Pecho Alto, Pelvis
        if (bGotHead) TargetPoints.Add(HeadLocation);
        if (bGotUpperChest) TargetPoints.Add(UpperChestLocation);
        if (bGotPelvis) TargetPoints.Add(PelvisLocation);

        if (bGotHead || bGotUpperChest || bGotPelvis)
        {
            bUsedBones = true;
            // UE_LOG(LogTemp, Verbose, TEXT("GetRaycastTargetPoints: Using %d bone locations for %s."), TargetPoints.Num(), *TargetActor->GetName());
        }
    }

    // Si no se usaron huesos (no es Character, no tiene Mesh, o huesos no encontrados), usar lógica anterior.
    if (!bUsedBones)
    {
        // UE_LOG(LogTemp, Warning, TEXT("GetRaycastTargetPoints: Bones not used for %s. Falling back to bounds/capsule logic."), *TargetActor->GetName());
        FVector ActorLocation = TargetActor->GetActorLocation();
        FVector UpVector = TargetActor->GetActorUpVector(); // Usar el UpVector del actor

        if (TargetCharacter && TargetCharacter->GetCapsuleComponent())
        {
            const UCapsuleComponent* Capsule = TargetCharacter->GetCapsuleComponent();
            const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();

            // Aproximaciones usando la cápsula, en orden: Cabeza, Pecho Alto, Pelvis
            TargetPoints.Add(ActorLocation + UpVector * (HalfHeight * 0.8f));   // Cabeza
            TargetPoints.Add(ActorLocation + UpVector * (HalfHeight * 0.4f));   // Pecho Alto (aproximación)
            TargetPoints.Add(ActorLocation);                                    // Pelvis (base de la cápsula)
        }
        else // Fallback para actores no-character o characters sin cápsula
        {
            FVector Origin, BoxExtent;
            TargetActor->GetActorBounds(true, Origin, BoxExtent);

            // Aproximaciones usando los bounds del actor, en orden: Cabeza, Pecho Alto, Pelvis
            TargetPoints.Add(Origin + UpVector * (BoxExtent.Z * 0.75f));       // Cabeza (parte superior)
            TargetPoints.Add(Origin);                                           // Pecho Alto (centro del bound)
            TargetPoints.Add(Origin - UpVector * (BoxExtent.Z * 0.75f));       // Pelvis (parte inferior)
        }
    }
    return TargetPoints;
}


bool UAISense_EnhancedSight::HasLineOfSight(
    const UWorld* World,
    const FVector& ViewPoint,
    const AActor* TargetActor,
    const AActor* IgnoredActorForTrace,
    const UAISenseConfig_EnhancedSight* SenseConfig) const
{
    if (!World || !TargetActor || !SenseConfig) return false;

    const int32 RequiredHits = FMath::Clamp(SenseConfig->RequiredRaycastHits, 1, 3);
    int32 SuccessfulHits = 0;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(IgnoredActorForTrace);
    if (const APawn* TargetPawn = Cast<const APawn>(TargetActor))
    {
        if (AController* Controller = TargetPawn->GetController())
        {
            QueryParams.AddIgnoredActor(Controller);
        }
    }


    if (RequiredHits <= 1 && SenseConfig->RequiredRaycastHits <= 1) // Comportamiento original si RequiredRaycastHits es 1 (o menos)
    {                                                              // y no se desea multi-raycast explícitamente desde config.
        FHitResult HitResult;
        const FVector TargetLocation = TargetActor->GetActorLocation(); // Traza al centro del actor para el caso simple
        bool bHit = World->LineTraceSingleByChannel(HitResult, ViewPoint, TargetLocation, ECC_Visibility, QueryParams);

        if (SenseConfig->bDrawDebug)
        {
            DrawDebugLine(World, ViewPoint, TargetLocation, bHit && HitResult.GetActor() != TargetActor ? FColor::Red : FColor::Green, false, 0.0f, 0, 1.0f);
        }
        return !bHit || (HitResult.GetActor() == TargetActor);
    }
    else // Lógica de multi-raycast (cuando RequiredRaycastHits > 1 o se prefiere siempre usar los puntos)
    {
        TArray<FVector> TargetPoints = GetRaycastTargetPoints(TargetActor);
        if (TargetPoints.Num() == 0)
        {
            // UE_LOG(LogTemp, Warning, TEXT("HasLineOfSight: No target points for %s."), *TargetActor->GetName());
            return false; // No hay puntos a los que trazar
        }


        for (const FVector& TargetPoint : TargetPoints)
        {
            FHitResult HitResult;
            bool bHit = World->LineTraceSingleByChannel(HitResult, ViewPoint, TargetPoint, ECC_Visibility, QueryParams);

            if (SenseConfig->bDrawDebug)
            {
                DrawDebugLine(World, ViewPoint, TargetPoint, bHit && HitResult.GetActor() != TargetActor ? FColor::Orange : FColor::Blue, false, 0.0f, 0, 0.5f);
                if (!bHit || (HitResult.GetActor() == TargetActor))
                {
                    DrawDebugPoint(World, TargetPoint, 10.f, FColor::Cyan, false, 0.0f);
                }
                else if (HitResult.GetActor()) {
                    DrawDebugPoint(World, HitResult.ImpactPoint, 10.f, FColor::Magenta, false, 0.0f);
                }
            }

            if (!bHit || (HitResult.GetActor() == TargetActor))
            {
                SuccessfulHits++;
            }
        }
        return SuccessfulHits >= RequiredHits;
    }
}


void UAISense_EnhancedSight::DrawDebugEnhancedFov(
    const UWorld* World,
    const FVector& ViewPoint,
    const FVector& ListenerHorizontalForward,
    const FVector& ListenerActualForward3D,
    const UAISenseConfig_EnhancedSight* SenseConfig, // Se recibe el Config completo
    float CachedApexOffset,
    const AActor* ListenerActorForDebug             // Para cálculo robusto de ListenerRight3D
) const
{
    // Ensure SenseConfig is valid and debug drawing is enabled.
    if (!SenseConfig || !SenseConfig->bDrawDebug || !World)
    {
        return;
    }

    // Colors and parameters for drawing.
    const FColor InitialFovColor = FColor::Green;
    const FColor LoseSightRadiusColor = FColor::Orange;
    const FColor SightRadiusDisplayColor = InitialFovColor;
    const FColor VerticalFovDebugColor = FColor::Yellow;
    const FColor DebugSphereColor = FColor::Red;

    const float DebugLifeTime = 0.0f;
    const float Thickness = 1.0f;
    const int32 ArcSegments = 24;
    const FVector HorizontalRotationAxis = FVector::UpVector;

    // --- Extracción de todos los parámetros necesarios DESDE SenseConfig ---
    const float InitialFovDegrees = SenseConfig->PeripheralVisionAngle;
    const float FinalFovDegrees = SenseConfig->FinalPeripheralVisionAngle;
    const float ThresholdHorizontalDist = SenseConfig->FinalPeripheralVisionAngleThreesholdDistance;
    const float SightRadiusFromConfig = SenseConfig->SightRadius;
    const float LoseSightRadiusFromConfig = SenseConfig->LoseSightRadius;
    const float VerticalFovDegrees = SenseConfig->VerticalPeripheralVisionAngle;
    // *** AQUÍ SE EXTRAEN MaxDistUp y MaxDistDown del SenseConfig ***
    const float ConfigMaxDistUp = SenseConfig->MaxDistUp;
    const float ConfigMaxDistDown = SenseConfig->MaxDistDown;

    // --- 1. Draw Horizontal FOV ---
    float HalfInitialFovDeg = InitialFovDegrees * 0.5f;
    FVector LeftEdgeDirInitial = ListenerHorizontalForward.RotateAngleAxis(-HalfInitialFovDeg, HorizontalRotationAxis);
    FVector RightEdgeDirInitial = ListenerHorizontalForward.RotateAngleAxis(HalfInitialFovDeg, HorizontalRotationAxis);
    FVector LeftEdgeEndInitial_AtThreshold = ViewPoint + LeftEdgeDirInitial * ThresholdHorizontalDist;
    FVector RightEdgeEndInitial_AtThreshold = ViewPoint + RightEdgeDirInitial * ThresholdHorizontalDist;

    DrawDebugLine(World, ViewPoint, LeftEdgeEndInitial_AtThreshold, InitialFovColor, false, DebugLifeTime, 0, Thickness);
    DrawDebugLine(World, ViewPoint, RightEdgeEndInitial_AtThreshold, InitialFovColor, false, DebugLifeTime, 0, Thickness);

    FVector VirtualOriginFinalFov = ViewPoint + ListenerHorizontalForward * CachedApexOffset;
    float HalfFinalFovDeg = FinalFovDegrees * 0.5f;
    // These points are on the cone originating from VirtualOriginFinalFov
    FVector LeftEdgeEnd_AtSightRadius = VirtualOriginFinalFov + ListenerHorizontalForward.RotateAngleAxis(-HalfFinalFovDeg, HorizontalRotationAxis) * SightRadiusFromConfig;
    FVector RightEdgeEnd_AtSightRadius = VirtualOriginFinalFov + ListenerHorizontalForward.RotateAngleAxis(HalfFinalFovDeg, HorizontalRotationAxis) * SightRadiusFromConfig;

    DrawDebugLine(World, LeftEdgeEndInitial_AtThreshold, LeftEdgeEnd_AtSightRadius, SightRadiusDisplayColor, false, DebugLifeTime, 0, Thickness);
    DrawDebugLine(World, RightEdgeEndInitial_AtThreshold, RightEdgeEnd_AtSightRadius, SightRadiusDisplayColor, false, DebugLifeTime, 0, Thickness);
    // Far horizontal arc is drawn from VirtualOrigin to match the far FOV cone shape for angles
    //DrawDebugArcManuallySimple(World, VirtualOriginFinalFov, SightRadiusFromConfig, HorizontalRotationAxis, ListenerHorizontalForward, HalfFinalFovDeg, ArcSegments, SightRadiusDisplayColor, DebugLifeTime, Thickness);
    DrawDebugArcManuallySimple(World, ViewPoint, SightRadiusFromConfig, HorizontalRotationAxis, ListenerHorizontalForward, HalfFinalFovDeg, ArcSegments, SightRadiusDisplayColor, DebugLifeTime, Thickness);

    // --- 2. Draw Vertical FOV with "Cut and Forward" Lines and Capped Arc ---
    FVector ListenerRight3D_Calculated;
    if (ListenerActorForDebug)
    {
        if (FMath::Abs(FVector::DotProduct(ListenerActualForward3D, FVector::UpVector)) > 0.99f)
        {
            ListenerRight3D_Calculated = ListenerActorForDebug->GetActorRightVector();
        }
        else
        {
            ListenerRight3D_Calculated = FVector::CrossProduct(ListenerActualForward3D, ListenerActorForDebug->GetActorUpVector()).GetSafeNormal();
            if (ListenerRight3D_Calculated.IsNearlyZero()) ListenerRight3D_Calculated = ListenerActorForDebug->GetActorRightVector();
        }
    }
    else { // Fallback if ListenerActorForDebug is somehow null
        ListenerRight3D_Calculated = FVector::CrossProduct(FVector::UpVector, ListenerActualForward3D).GetSafeNormal();
        if (ListenerRight3D_Calculated.IsNearlyZero()) ListenerRight3D_Calculated = FVector(0.f, 1.f, 0.f); // World Y as fallback
    }

    float HalfVerticalFovDeg = VerticalFovDegrees * 0.5f;
    float VerticalDrawExtent = SightRadiusFromConfig; // Max length for vertical FOV edge lines

    // Top Edge
    FVector TopEdgeDir = ListenerActualForward3D.RotateAngleAxis(HalfVerticalFovDeg, ListenerRight3D_Calculated).GetSafeNormal();
    FVector TopEndPoint_NoLimit = ViewPoint + TopEdgeDir * VerticalDrawExtent;
    FVector ActualTopVisualEndPoint = TopEndPoint_NoLimit;

    if (TopEdgeDir.Z > KINDA_SMALL_NUMBER && ConfigMaxDistUp > 0) // Use ConfigMaxDistUp
    {
        float DistToCeilingPlane = ConfigMaxDistUp / TopEdgeDir.Z;
        if (DistToCeilingPlane > 0 && DistToCeilingPlane < VerticalDrawExtent)
        {
            FVector IntersectionPointCeiling = ViewPoint + TopEdgeDir * DistToCeilingPlane;
            DrawDebugLine(World, ViewPoint, IntersectionPointCeiling, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f);
            ActualTopVisualEndPoint = FVector(TopEndPoint_NoLimit.X, TopEndPoint_NoLimit.Y, IntersectionPointCeiling.Z);
            DrawDebugLine(World, IntersectionPointCeiling, ActualTopVisualEndPoint, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f);
        }
        else { DrawDebugLine(World, ViewPoint, ActualTopVisualEndPoint, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f); }
    }
    else { DrawDebugLine(World, ViewPoint, ActualTopVisualEndPoint, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f); }

    // Bottom Edge
    FVector BottomEdgeDir = ListenerActualForward3D.RotateAngleAxis(-HalfVerticalFovDeg, ListenerRight3D_Calculated).GetSafeNormal();
    FVector BottomEndPoint_NoLimit = ViewPoint + BottomEdgeDir * VerticalDrawExtent;
    FVector ActualBottomVisualEndPoint = BottomEndPoint_NoLimit;

    if (BottomEdgeDir.Z < -KINDA_SMALL_NUMBER && ConfigMaxDistDown > 0) // Use ConfigMaxDistDown
    {
        float DistToFloorPlane = -ConfigMaxDistDown / BottomEdgeDir.Z;
        if (DistToFloorPlane > 0 && DistToFloorPlane < VerticalDrawExtent)
        {
            FVector IntersectionPointFloor = ViewPoint + BottomEdgeDir * DistToFloorPlane;
            DrawDebugLine(World, ViewPoint, IntersectionPointFloor, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f);
            ActualBottomVisualEndPoint = FVector(BottomEndPoint_NoLimit.X, BottomEndPoint_NoLimit.Y, IntersectionPointFloor.Z);
            DrawDebugLine(World, IntersectionPointFloor, ActualBottomVisualEndPoint, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f);
        }
        else { DrawDebugLine(World, ViewPoint, ActualBottomVisualEndPoint, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f); }
    }
    else { DrawDebugLine(World, ViewPoint, ActualBottomVisualEndPoint, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.8f); }

    // Use the Capped Arc for vertical FOV visualization
    DrawDebugArcManuallyCapped(
        World, ViewPoint, VerticalDrawExtent, ListenerRight3D_Calculated, ListenerActualForward3D,
        HalfVerticalFovDeg, ArcSegments, VerticalFovDebugColor, DebugLifeTime, Thickness * 0.8f,
        ConfigMaxDistUp, ConfigMaxDistDown // Pass the Z offsets from config
    );

    // MaxDistUp/Down horizontal indicator lines
    float IndicationLineLength = 100.0f;
    FVector UpLimitVisualCenter = ViewPoint + FVector(0, 0, ConfigMaxDistUp);
    FVector DownLimitVisualCenter = ViewPoint - FVector(0, 0, ConfigMaxDistDown);
    FVector HorizontalRightIndicator = FVector::CrossProduct(FVector::UpVector, ListenerHorizontalForward).GetSafeNormal();
    if (HorizontalRightIndicator.IsNearlyZero()) HorizontalRightIndicator = FVector(0, 1, 0); // Fallback

    DrawDebugLine(World, UpLimitVisualCenter - HorizontalRightIndicator * IndicationLineLength, UpLimitVisualCenter + HorizontalRightIndicator * IndicationLineLength, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.5f);
    DrawDebugLine(World, DownLimitVisualCenter - HorizontalRightIndicator * IndicationLineLength, DownLimitVisualCenter + HorizontalRightIndicator * IndicationLineLength, VerticalFovDebugColor, false, DebugLifeTime, 0, Thickness * 0.5f);
}


void UAISense_EnhancedSight::ProcessSight()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AIPerception::FListenerMap* Listeners = GetListeners();
    if (!Listeners || Listeners->Num() == 0) return;

    for (AIPerception::FListenerMap::TIterator ListenerIt(*Listeners); ListenerIt; ++ListenerIt)
    {
        FPerceptionListener& Listener = ListenerIt->Value;
        const AActor* ListenerActor = Listener.GetBodyActor();
        UAIPerceptionComponent* PerceptionComponent = Listener.Listener.Get();

        if (!ListenerActor || !PerceptionComponent) continue;

        const UAISenseConfig_EnhancedSight* MyConfig = Cast<const UAISenseConfig_EnhancedSight>(PerceptionComponent->GetSenseConfig(GetSenseID()));
        if (!MyConfig) continue;

        const float InitialFovDegrees = MyConfig->PeripheralVisionAngle;
        const float FinalFovDegrees = MyConfig->FinalPeripheralVisionAngle;
        const float ThresholdHorizontalDist = MyConfig->FinalPeripheralVisionAngleThreesholdDistance;
        const float SightRadiusFromConfig = MyConfig->SightRadius;
        const float LoseSightRadiusFromConfig = MyConfig->LoseSightRadius;

        const float ThresholdHorizontalDistSq = FMath::Square(ThresholdHorizontalDist);
        const float InitialFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(InitialFovDegrees * 0.5f));
        const float FinalFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(FinalFovDegrees * 0.5f));
        const float CachedApexOffset = CalculateFovApexOffset(InitialFovDegrees, FinalFovDegrees, ThresholdHorizontalDist);

        FVector ViewPoint;
        FVector ListenerActualForward3D;
        FRotator ViewRotation; // Moved FRotator declaration out

        APawn* ListenerPawn = Cast<APawn>(const_cast<AActor*>(ListenerActor)); // Const_cast to call non-const pawn methods
        if (ListenerPawn && ListenerPawn->IsPlayerControlled() && ListenerPawn->GetController())
        {
            ListenerPawn->GetController()->GetPlayerViewPoint(ViewPoint, ViewRotation);
            ListenerActualForward3D = ViewRotation.Vector();
        }
        else if (ListenerPawn)
        {
            ListenerPawn->GetActorEyesViewPoint(ViewPoint, ViewRotation);
            ListenerActualForward3D = ViewRotation.Vector();
        }
        else
        {
            ViewPoint = ListenerActor->GetActorLocation();
            ListenerActualForward3D = ListenerActor->GetActorForwardVector();
            ViewRotation = ListenerActualForward3D.Rotation(); // Initialize ViewRotation for consistency
        }


        FVector ListenerHorizontalForward = ListenerActualForward3D;
        ListenerHorizontalForward.Z = 0.0f;
        if (!ListenerHorizontalForward.Normalize())
        {
            ListenerHorizontalForward = ListenerActor->GetActorForwardVector();
            ListenerHorizontalForward.Z = 0.0f;
            if (!ListenerHorizontalForward.Normalize()) {
                ListenerHorizontalForward = FVector::ForwardVector;
            }
        }

        if (MyConfig->bDrawDebug)
        {
            DrawDebugEnhancedFov(
                World,
                ViewPoint,
                ListenerHorizontalForward,
                ListenerActualForward3D,
                MyConfig,
                CachedApexOffset,
                ListenerActor // Pass ListenerActor for robust RightVector calculation in debug
            );
        }

        for (TActorIterator<APawn> TargetIt(World); TargetIt; ++TargetIt)
        {
            APawn* TargetActorAsPawn = *TargetIt;
            if (!TargetActorAsPawn || TargetActorAsPawn == ListenerActor) continue; // Ignore self
            AActor* TargetActor = TargetActorAsPawn;

            const FActorPerceptionInfo* TargetPerceptionInfo = PerceptionComponent->GetActorInfo(*TargetActor);
            bool bWasSuccessfullySensedPreviously = false;
            if (TargetPerceptionInfo)
            {
                const int32 SenseIndex = GetSenseID().Index;
                if (TargetPerceptionInfo->LastSensedStimuli.IsValidIndex(SenseIndex))
                {
                    bWasSuccessfullySensedPreviously = TargetPerceptionInfo->LastSensedStimuli[SenseIndex].WasSuccessfullySensed();
                }
            }

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

            float DistanceToTarget3D;
            FVector DirectionToTarget3DNormalized;

            bool bPassesAllChecks = IsInFovAndRange(
                ViewPoint,
                ListenerHorizontalForward,
                ListenerActualForward3D,
                MyConfig,
                MaxRangeSqForCheck,
                TargetActor,
                ThresholdHorizontalDistSq,
                InitialFovHorizontalDot,
                FinalFovHorizontalDot,
                CachedApexOffset,
                DistanceToTarget3D,
                DirectionToTarget3DNormalized
            );

            bool bHasLineOfSightToTarget = false;
            if (bPassesAllChecks)
            {
                bHasLineOfSightToTarget = HasLineOfSight(World, ViewPoint, TargetActor, ListenerActor, MyConfig);
            }
            const bool bIsCurrentlyVisible = bPassesAllChecks && bHasLineOfSightToTarget;

            FAIStimulus::FResult CurrentSenseResult = bIsCurrentlyVisible ? FAIStimulus::SensingSucceeded : FAIStimulus::SensingFailed;
            bool bShouldReportStimulus = false;

            if (bIsCurrentlyVisible)
            {
                bShouldReportStimulus = true;
            }
            else
            {
                if (bWasSuccessfullySensedPreviously)
                {
                    bShouldReportStimulus = true;
                }
            }

            if (bShouldReportStimulus)
            {
                const FVector ActualTargetLocation = TargetActor->GetActorLocation();
                FAIStimulus Stimulus(
                    *this,
                    bIsCurrentlyVisible ? 1.0f : 0.0f,
                    ActualTargetLocation,
                    ViewPoint,
                    CurrentSenseResult,
                    GetSenseID().Name // Use the sense's FName as tag
                );

                Listener.RegisterStimulus(TargetActor, Stimulus);
            }
        }
    }
}