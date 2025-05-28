#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "AISense_EnhancedSight.generated.h"

UCLASS(ClassGroup = AI)
class STEALTH_API UAISense_EnhancedSight : public UAISense
{
    GENERATED_BODY()

public:
    UAISense_EnhancedSight();

    virtual float Update() override;
    virtual void PostInitProperties() override;

private:
    void ProcessSight();

    float CalculateFovApexOffset(
        float InitialFovDegrees,
        float FinalFovDegrees,
        float ThresholdHorizontalDistance) const;

    // Nueva funci�n para dibujar el FOV de debug
    void DrawDebugEnhancedFov(
        const UWorld* World,
        const FVector& ViewPoint,
        const FVector& ListenerHorizontalForward,
        float InitialFovDegrees,
        float FinalFovDegrees,
        float ThresholdHorizontalDist,
        float SightRadiusFromConfig,
        float LoseSightRadiusFromConfig,
        float CachedApexOffset,
        bool  DrawDebug
    ) const; // Marcada como const ya que no modifica el estado del sentido

    // La funci�n est�tica para dibujar arcos permanece igual,
    // su declaraci�n no es necesaria aqu� si est� definida como 'static' en el .cpp
    // static void DrawDebugArcManually(...); 

protected:
    bool IsInFovAndRange(
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
    ) const;

    bool HasLineOfSight(
        const UWorld* World,
        const FVector& ViewPoint,
        const AActor* TargetActor,
        const AActor* IgnoredActorForTrace
    ) const;
};