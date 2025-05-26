#include "AISenseConfig_EnhancedSight.h"

UAISenseConfig_EnhancedSight::UAISenseConfig_EnhancedSight()
{
    Implementation = UAISense_EnhancedSight::StaticClass();
    SightRadius = 2000.0f;
    LoseSightRadius = 2500.0f;
    PeripheralVisionAngle = 90.0f;
}

TSubclassOf<UAISense> UAISenseConfig_EnhancedSight::GetSenseImplementation() const
{
    return Implementation;
}
