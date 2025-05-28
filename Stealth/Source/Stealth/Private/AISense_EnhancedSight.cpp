// AISense_EnhancedSight.cpp

#include "AISense_EnhancedSight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig.h"
#include "AISenseConfig_EnhancedSight.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h" // For DrawDebugLine, DrawDebugPoint
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/UnrealMathUtility.h"     // For FMath, UE_PI, UE_BIG_NUMBER
#include "DrawDebugHelpers.h"           // For additional debug drawing (though DrawDebugArcManually is custom)

// Constructor
UAISense_EnhancedSight::UAISense_EnhancedSight()
{
    UE_LOG(LogTemp, Warning, TEXT("EnhancedSight Constructor() called")); // User's existing log
    RequestImmediateUpdate();
    NotifyType = EAISenseNotifyType::OnEveryPerception;
}

// PostInitProperties
void UAISense_EnhancedSight::PostInitProperties()
{
    Super::PostInitProperties();
    UE_LOG(LogTemp, Warning, TEXT("UAISense_EnhancedSight::PostInitProperties() CALLED")); // User's existing log
}

// Update is called by the AIPerceptionSystem to perform sight checks.
float UAISense_EnhancedSight::Update()
{
    // Removed recurrent log from here to reduce spam.
    ProcessSight();
    return 0.f; // User's current setting. Consider a small positive value (e.g., 0.1f) to control update frequency.
}

// Calculates the virtual apex offset for the final FOV cone.
// This ensures the final FOV cone's edges meet the initial FOV cone's edges at the threshold distance.
float UAISense_EnhancedSight::CalculateFovApexOffset(
    float InitialFovDegrees,
    float FinalFovDegrees,
    float ThresholdRadialDistance) const
{
    // Sanity checks for input parameters.
    if (ThresholdRadialDistance < KINDA_SMALL_NUMBER ||
        InitialFovDegrees < KINDA_SMALL_NUMBER || InitialFovDegrees >= 179.9f ||
        FinalFovDegrees < KINDA_SMALL_NUMBER || FinalFovDegrees >= 179.9f)
    {
        return 0.0f; // Return no offset for invalid or extreme FOV values.
    }

    const float HalfInitialFovRad = FMath::DegreesToRadians(InitialFovDegrees * 0.5f);
    const float HalfFinalFovRad = FMath::DegreesToRadians(FinalFovDegrees * 0.5f);

    // If either half-angle is close to 90 degrees (FOV close to 180), Tangent becomes problematic.
    if (FMath::Abs(HalfInitialFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER ||
        FMath::Abs(HalfFinalFovRad - (UE_PI * 0.5f)) < KINDA_SMALL_NUMBER) {
        return 0.0f; // No offset for FOVs near 180 degrees.
    }

    float TanHalfInitial = FMath::Tan(HalfInitialFovRad);
    float TanHalfFinal = FMath::Tan(HalfFinalFovRad);

    // Calculate geometry for the initial FOV cone.
    // ActualLastRight: Half-width of the initial FOV sector's base.
    // ThresholdRadialDistance is treated as the slant length to the edge of this sector.
    float ActualLastRight = FMath::IsNearlyZero(TanHalfInitial) ? 0.0f : (ThresholdRadialDistance * FMath::Sin(HalfInitialFovRad));

    // ActualLastForward: Forward projection distance to the plane containing the base of the initial cone.
    float ActualLastForward = ThresholdRadialDistance * FMath::Cos(HalfInitialFovRad);

    // Calculate ForwardDiff for the final FOV.
    // This is the forward distance the final FOV's virtual apex would need to be from the shared base plane
    // to achieve the same width (ActualLastRight) at that plane.
    float ForwardDiff;
    if (FMath::IsNearlyZero(TanHalfFinal)) // Final FOV is effectively 0 degrees.
    {
        ForwardDiff = FMath::IsNearlyZero(ActualLastRight) ? ActualLastForward : UE_BIG_NUMBER;
    }
    else
    {
        ForwardDiff = ActualLastRight / TanHalfFinal;
    }

    // Clamp ForwardDiff to prevent extreme offset values due to float precision with UE_BIG_NUMBER.
    if (ForwardDiff > (UE_BIG_NUMBER / 2.0f))
    {
        ForwardDiff = (UE_BIG_NUMBER / 2.0f);
    }
    else if (ForwardDiff < (-UE_BIG_NUMBER / 2.0f))
    {
        ForwardDiff = (-UE_BIG_NUMBER / 2.0f);
    }

    // The offset is the difference in forward projection of the two cones.
    return ActualLastForward - ForwardDiff;
}

// Checks if a target actor is within the defined Field of View and range.
bool UAISense_EnhancedSight::IsInFovAndRange(
    const FVector& ViewPoint,
    const FVector& ListenerHorizontalForwardNormalized,
    float MaxHorizontalRangeSquared, // Max range, interpreted as horizontal, already squared.
    const AActor* TargetActor,
    float ThresholdHorizontalDistanceSq, // Threshold for FOV switch, horizontal, already squared.
    float InitialFovHorizontalDotProduct,  // Dot product for initial FOV angle.
    float FinalFovHorizontalDotProduct,    // Dot product for final FOV angle.
    float CachedApexOffset,                // Pre-calculated virtual apex offset.
    float& OutDistanceToTarget3D,          // Output: Actual 3D distance to target.
    FVector& OutDirectionToTarget3DNormalized // Output: Normalized 3D direction to target.
) const
{
    if (!TargetActor)
    {
        return false;
    }

    const FVector TargetLocation = TargetActor->GetActorLocation();
    FVector DirectionToTarget3D = TargetLocation - ViewPoint;
    const float DistanceToTargetSq3D = DirectionToTarget3D.SizeSquared();

    // Populate 3D output parameters first.
    OutDistanceToTarget3D = FMath::Sqrt(DistanceToTargetSq3D);
    if (OutDistanceToTarget3D < KINDA_SMALL_NUMBER) // Target is at the viewpoint.
    {
        OutDirectionToTarget3DNormalized = ListenerHorizontalForwardNormalized; // Default direction.
        return true; // Considered within range and FOV if at the same point.
    }
    OutDirectionToTarget3DNormalized = DirectionToTarget3D / OutDistanceToTarget3D;

    // Perform calculations on the horizontal plane (Z-plane of ViewPoint).
    FVector TargetLocationHorizontal = TargetLocation;
    TargetLocationHorizontal.Z = ViewPoint.Z;

    FVector DirectionToTargetHorizontal = TargetLocationHorizontal - ViewPoint;
    const float DistanceToTargetHorizontalSq = DirectionToTargetHorizontal.SizeSquared();

    // Primary range check using horizontal distance.
    if (DistanceToTargetHorizontalSq > MaxHorizontalRangeSquared)
    {
        return false;
    }

    FVector DirectionToTargetHorizontalNormalized = DirectionToTargetHorizontal.GetSafeNormal();
    if (DirectionToTargetHorizontal.IsNearlyZero()) // Target is horizontally at the same spot as viewpoint.
    {
        return true; // Within range and no horizontal angle to check.
    }

    float RelevantHorizontalDotProduct;
    FVector OriginForAngleCheckHorizontal = ViewPoint; // Default origin for angle check.

    // Determine if target is within the initial threshold distance.
    if (DistanceToTargetHorizontalSq <= ThresholdHorizontalDistanceSq)
    {
        // Within threshold: use initial FOV parameters.
        RelevantHorizontalDotProduct = InitialFovHorizontalDotProduct;
        // Angle check origin remains ViewPoint.
    }
    else
    {
        // Beyond threshold: use final FOV parameters and the shifted apex.
        RelevantHorizontalDotProduct = FinalFovHorizontalDotProduct;
        OriginForAngleCheckHorizontal = ViewPoint + ListenerHorizontalForwardNormalized * CachedApexOffset;

        // Recalculate direction from the new virtual origin for angle check.
        DirectionToTargetHorizontal = TargetLocationHorizontal - OriginForAngleCheckHorizontal;
        DirectionToTargetHorizontalNormalized = DirectionToTargetHorizontal.GetSafeNormal();

        if (DirectionToTargetHorizontal.IsNearlyZero()) // Target is at the virtual apex.
        {
            return true;
        }
    }

    // Perform the horizontal angle check.
    const float DotProductHorizontal = FVector::DotProduct(ListenerHorizontalForwardNormalized, DirectionToTargetHorizontalNormalized);
    return DotProductHorizontal >= RelevantHorizontalDotProduct;
}

// Checks for a direct line of sight between ViewPoint and TargetActor.
bool UAISense_EnhancedSight::HasLineOfSight(
    const UWorld* World,
    const FVector& ViewPoint,
    const AActor* TargetActor,
    const AActor* IgnoredActorForTrace // Typically the sensing actor itself.
) const
{
    if (!World || !TargetActor)
    {
        return false;
    }

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(IgnoredActorForTrace); // Ignore self in trace.

    const FVector TargetLocation = TargetActor->GetActorLocation(); // Or a specific socket on the target.

    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        ViewPoint,
        TargetLocation,
        ECC_Visibility, // Standard visibility channel.
        QueryParams
    );

    // If no hit, or hit actor is the target, then LoS is clear.
    return !bHit || (HitResult.GetActor() == TargetActor);
}

static void DrawDebugArcManually(
    const UWorld* InWorld,
    const FVector& Center,
    float Radius,
    const FVector& Axis,
    const FVector& ForwardVector,
    float HalfAngleDegrees,
    int32 NumSegments,
    const FColor& Color,
    float LifeTime,
    float Thickness
)
{
    if (!InWorld || NumSegments <= 0 || Radius < KINDA_SMALL_NUMBER || HalfAngleDegrees < KINDA_SMALL_NUMBER)
    {
        return;
    }
    const float HalfAngleRad = FMath::DegreesToRadians(HalfAngleDegrees);
    const float AngleStep = (NumSegments > 0 && !FMath::IsNearlyZero(HalfAngleRad * 2.0f))
        ? ((HalfAngleRad * 2.0f) / static_cast<float>(NumSegments))
        : 0.0f;
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

void UAISense_EnhancedSight::DrawDebugEnhancedFov(
    const UWorld* World,
    const FVector& ViewPoint,
    const FVector& ListenerHorizontalForward,
    float InitialFovDegrees,
    float FinalFovDegrees,
    float ThresholdHorizontalDist,
    float SightRadiusFromConfig,
    float LoseSightRadiusFromConfig,
    float CachedApexOffset,
    bool  DrawDebug) const
{

    if (!DrawDebug) return;

    const FColor InitialFovColor = FColor::Green;
    const FColor LoseSightRadiusColor = FColor::Orange;
    const FColor SightRadiusDisplayColor = InitialFovColor;

    const float DebugLifeTime = 0.0f;
    const float Thickness = 1.0f;
    const int32 ArcSegments = 24;
    const FVector RotationAxis = FVector::UpVector;

    // 1. Draw Initial FOV Segment
    float HalfInitialFovDeg = InitialFovDegrees * 0.5f;
    FVector LeftEdgeDirInitial = ListenerHorizontalForward.RotateAngleAxis(-HalfInitialFovDeg, RotationAxis);
    FVector RightEdgeDirInitial = ListenerHorizontalForward.RotateAngleAxis(HalfInitialFovDeg, RotationAxis);
    FVector LeftEdgeEndInitial_AtThreshold = ViewPoint + LeftEdgeDirInitial * ThresholdHorizontalDist;
    FVector RightEdgeEndInitial_AtThreshold = ViewPoint + RightEdgeDirInitial * ThresholdHorizontalDist;

    DrawDebugLine(World, ViewPoint, LeftEdgeEndInitial_AtThreshold, InitialFovColor, false, DebugLifeTime, 0, Thickness);
    DrawDebugLine(World, ViewPoint, RightEdgeEndInitial_AtThreshold, InitialFovColor, false, DebugLifeTime, 0, Thickness);
    DrawDebugArcManually(World, ViewPoint, ThresholdHorizontalDist, RotationAxis, ListenerHorizontalForward, HalfInitialFovDeg, ArcSegments, InitialFovColor, DebugLifeTime, Thickness);

    // 2. Draw Extended FOV Segment
    FVector VirtualOriginFinalFov = ViewPoint + ListenerHorizontalForward * CachedApexOffset;
    float HalfFinalFovDeg = FinalFovDegrees * 0.5f;

    FVector LeftEdgeEnd_AtSightRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(-HalfFinalFovDeg, RotationAxis) * SightRadiusFromConfig;
    FVector RightEdgeEnd_AtSightRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(HalfFinalFovDeg, RotationAxis) * SightRadiusFromConfig;

    DrawDebugLine(World, LeftEdgeEndInitial_AtThreshold, LeftEdgeEnd_AtSightRadius_FromViewPoint, SightRadiusDisplayColor, false, DebugLifeTime, 0, Thickness);
    DrawDebugLine(World, RightEdgeEndInitial_AtThreshold, RightEdgeEnd_AtSightRadius_FromViewPoint, SightRadiusDisplayColor, false, DebugLifeTime, 0, Thickness);
    DrawDebugArcManually(World, ViewPoint, SightRadiusFromConfig, RotationAxis, ListenerHorizontalForward, HalfFinalFovDeg, ArcSegments, SightRadiusDisplayColor, DebugLifeTime, Thickness);

    if (LoseSightRadiusFromConfig > SightRadiusFromConfig)
    {
        FVector LeftEdgeEnd_AtLoseRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(-HalfFinalFovDeg, RotationAxis) * LoseSightRadiusFromConfig;
        FVector RightEdgeEnd_AtLoseRadius_FromViewPoint = ViewPoint + ListenerHorizontalForward.RotateAngleAxis(HalfFinalFovDeg, RotationAxis) * LoseSightRadiusFromConfig;

        DrawDebugLine(World, LeftEdgeEnd_AtSightRadius_FromViewPoint, LeftEdgeEnd_AtLoseRadius_FromViewPoint, LoseSightRadiusColor, false, DebugLifeTime, 0, Thickness * 0.7f);
        DrawDebugLine(World, RightEdgeEnd_AtSightRadius_FromViewPoint, RightEdgeEnd_AtLoseRadius_FromViewPoint, LoseSightRadiusColor, false, DebugLifeTime, 0, Thickness * 0.7f);
        DrawDebugArcManually(World, ViewPoint, LoseSightRadiusFromConfig, RotationAxis, ListenerHorizontalForward, HalfFinalFovDeg, ArcSegments, LoseSightRadiusColor, DebugLifeTime, Thickness * 0.7f);
    }

    if (FMath::Abs(CachedApexOffset) > KINDA_SMALL_NUMBER)
    {
        DrawDebugPoint(World, VirtualOriginFinalFov, 15.f, FColor::Magenta, false, DebugLifeTime);
    }
}


void UAISense_EnhancedSight::ProcessSight()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const bool bDrawDebugFov = true; // Sigue siendo el control maestro aquí

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

        // Parámetros de Configuración del FOV (sin cambios)
        const float InitialFovDegrees = MyConfig->PeripheralVisionAngle;
        const float FinalFovDegrees = MyConfig->FinalPeripheralVisionAngle;
        const float ThresholdHorizontalDist = MyConfig->FinalPeripheralVisionAngleThreesholdDistance;
        const float SightRadiusFromConfig = MyConfig->SightRadius;
        const float LoseSightRadiusFromConfig = MyConfig->LoseSightRadius;

        // Pre-cálculos (sin cambios)
        const float ThresholdHorizontalDistSq = FMath::Square(ThresholdHorizontalDist);
        const float InitialFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(InitialFovDegrees * 0.5f));
        const float FinalFovHorizontalDot = FMath::Cos(FMath::DegreesToRadians(FinalFovDegrees * 0.5f));

        const float CachedApexOffset = CalculateFovApexOffset(
            InitialFovDegrees,
            FinalFovDegrees,
            ThresholdHorizontalDist
        );

        // ViewPoint y ListenerForward (sin cambios)
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

        if (bDrawDebugFov && World) // El control maestro sigue aquí
        {
            DrawDebugEnhancedFov(
                World,
                ViewPoint,
                ListenerHorizontalForward,
                InitialFovDegrees,
                FinalFovDegrees,
                ThresholdHorizontalDist,
                SightRadiusFromConfig,
                LoseSightRadiusFromConfig,
                CachedApexOffset,
                MyConfig->bDrawDebug
            );
        }

        // Bucle de Objetivos y Lógica de Percepción (sin cambios)
        for (TActorIterator<APawn> TargetIt(World); TargetIt; ++TargetIt)
        {
            APawn* TargetActorAsPawn = *TargetIt;

            if (!TargetActorAsPawn || TargetActorAsPawn == ListenerActor)
            {
                continue;
            }

            AActor* TargetActor = TargetActorAsPawn;

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

            float DistanceToTarget;
            FVector DirectionToTarget;

            bool bIsInFovAndRange = IsInFovAndRange(
                ViewPoint,
                ListenerHorizontalForward,
                MaxRangeSqForCheck,
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