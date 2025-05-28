#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig.h"
#include "AISense_EnhancedSight.h" // Asegúrate de que la ruta es correcta
#include "AISenseConfig_EnhancedSight.generated.h"

UCLASS(meta = (DisplayName = "Enhanced Sight Config")) // Nombre para el editor
class STEALTH_API UAISenseConfig_EnhancedSight : public UAISenseConfig
{
    GENERATED_BODY()

public:
    UAISenseConfig_EnhancedSight();

    virtual TSubclassOf<UAISense> GetSenseImplementation() const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
    TSubclassOf<UAISense_EnhancedSight> Implementation;

    /** Maximum distance at which a pawn can initially see a target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = "0.0"))
    float SightRadius;

    /** Distance at which a pawn loses sight of a previously seen target. Usually > SightRadius. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = "0.0"))
    float LoseSightRadius;

    /** Initial peripheral vision angle in degrees (total cone angle). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = "0.0", UIMax = "360.0"))
    float PeripheralVisionAngle;

    /** Final peripheral vision angle in degrees (total cone angle) after the threshold distance. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = "0.0", UIMax = "360.0"))
    float FinalPeripheralVisionAngle;

    /** Horizontal distance at which the FOV angle transitions from PeripheralVisionAngle to FinalPeripheralVisionAngle. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = "0.0"))
    float FinalPeripheralVisionAngleThreesholdDistance; // Typo mantenido por consistencia con tu código

    /** Maximum vertical distance upwards the pawn can see. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = "0.0"))
    float MaxDistUp;

    /** Maximum vertical distance downwards the pawn can see. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = "0.0"))
    float MaxDistDown;

    /** If true, debug shapes for this sense will be drawn in the world. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug", config)
    bool bDrawDebug;
};