#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig.h"
#include "AISense_EnhancedSight.h"
#include "AISenseConfig_EnhancedSight.generated.h"

UCLASS(meta = (DisplayName = "Enhanced Sight"))
class STEALTH_API UAISenseConfig_EnhancedSight : public UAISenseConfig
{
    GENERATED_BODY()

public:
    UAISenseConfig_EnhancedSight();

    virtual TSubclassOf<UAISense> GetSenseImplementation() const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
    TSubclassOf<UAISense_EnhancedSight> Implementation;

    /** Distance a pawn can see */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
    float SightRadius;

    /** Distance a pawn loses sight */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
    float LoseSightRadius;

    /** Angle of vision */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
    float PeripheralVisionAngle;

    // Otros parámetros personalizados
};
