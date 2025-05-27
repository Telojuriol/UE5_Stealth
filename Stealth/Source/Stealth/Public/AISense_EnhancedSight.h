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

protected: // O public, según cómo se llame desde ProcessSight
    bool IsInFovAndRange(
        const FVector& ViewPoint,
        const FVector& ListenerHorizontalForwardNormalized, // Vector adelante del listener, ya aplanado y normalizado
        float MaxRangeSquared,                      // Rango máximo al cuadrado para evitar Sqrt
        const AActor* TargetActor,
        float ThresholdHorizontalDistanceSq,        // Distancia umbral horizontal al cuadrado
        float InitialFovHorizontalDotProduct,       // cos(InitialFOV_degrees / 2)
        float FinalFovHorizontalDotProduct,         // cos(FinalFOV_degrees / 2)
        float CachedApexOffset,                     // El 'diff' precalculado
        float& OutDistanceToTarget,                 // Salida: Distancia 3D
        FVector& OutDirectionToTargetNormalized     // Salida: Dirección 3D normalizada
    ) const;

    float CalculateFovApexOffset(
        float InitialFovDegrees,
        float FinalFovDegrees,
        float ThresholdHorizontalDistance) const;

    // Devuelve true si hay una línea de visión directa desde ViewPoint hasta TargetActor.
    bool HasLineOfSight(
        const UWorld* World,
        const FVector& ViewPoint,
        const AActor* TargetActor,
        const AActor* IgnoredActorForTrace // El actor que realiza el trazado (la IA)
    ) const;
};
