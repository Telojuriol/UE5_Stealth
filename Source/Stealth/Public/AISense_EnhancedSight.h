// AISense_EnhancedSight.h

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "AISense_EnhancedSight.generated.h"

// Forward declaration for your specific config class
class UAISenseConfig_EnhancedSight;
class ACharacter; // Forward declare ACharacter

UCLASS(ClassGroup = AI)
class STEALTH_API UAISense_EnhancedSight : public UAISense
{
    GENERATED_BODY()

public:
    UAISense_EnhancedSight();

    virtual float Update() override;
    virtual void PostInitProperties() override;

private:
    // Main processing function called by Update
    void ProcessSight();

    // Calculates the apex offset for the final (far) FOV cone for horizontal checks
    float CalculateFovApexOffset(
        float InitialFovDegrees,
        float FinalFovDegrees,
        float ThresholdRadialDistance) const;

    // Draws the complete debug visualization for the FOV (horizontal and vertical)
    void DrawDebugEnhancedFov(
        const UWorld* World,
        const FVector& ViewPoint,
        const FVector& ListenerHorizontalForward,     // For horizontal FOV drawing
        const FVector& ListenerActualForward3D,       // For vertical FOV drawing
        const UAISenseConfig_EnhancedSight* SenseConfig, // To access all FOV parameters
        float CachedApexOffset,
        const AActor* ListenerActorForDebug     // For robust RightVector calculation in vertical debug
    ) const;

protected:
    // Checks if a target is within the combined horizontal and vertical FOV, and range limits
    bool IsInFovAndRange(
        const FVector& ViewPoint,
        const FVector& ListenerHorizontalForwardNormalized, // For horizontal FOV check
        const FVector& ListenerActualForward3D,             // For vertical FOV check
        const UAISenseConfig_EnhancedSight* SenseConfig,  // To access all FOV parameters including vertical
        float MaxHorizontalRangeSquared,                  // Max range for horizontal checks, already squared
        const AActor* TargetActor,
        float ThresholdHorizontalDistanceSq,     // Threshold for horizontal FOV angle switch, squared
        float InitialFovHorizontalDotProduct,    // Precalculated dot product for initial horizontal FOV
        float FinalFovHorizontalDotProduct,      // Precalculated dot product for final horizontal FOV
        float CachedApexOffset,                  // Pre-calculated virtual apex offset for horizontal FOV
        float& OutDistanceToTarget3D,            // Output: Actual 3D distance
        FVector& OutDirectionToTarget3DNormalized // Output: Normalized 3D direction
    ) const;

    // Checks for a direct line of sight to the target
    bool HasLineOfSight(
        const UWorld* World,
        const FVector& ViewPoint,
        const AActor* TargetActor,
        const AActor* IgnoredActorForTrace,
        const UAISenseConfig_EnhancedSight* SenseConfig // Added SenseConfig
    ) const;

private:
    // Helper to get specific target points for multi-raycast
    TArray<FVector> GetRaycastTargetPoints(const AActor* TargetActor) const;
};