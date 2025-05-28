// AISenseConfig_EnhancedSight.cpp

#include "AISenseConfig_EnhancedSight.h"
#include "AISense_EnhancedSight.h"

UAISenseConfig_EnhancedSight::UAISenseConfig_EnhancedSight()
{
    Implementation = UAISense_EnhancedSight::StaticClass();

    // Default values for sense parameters
    SightRadius = 2000.0f;
    LoseSightRadius = 2500.0f;
    PeripheralVisionAngle = 160.0f;
    FinalPeripheralVisionAngle = 90.0f;
    FinalPeripheralVisionAngleThreesholdDistance = 1000.0f;

    VerticalPeripheralVisionAngle = 170.0f;
    MaxDistUp = 1000.0f;
    MaxDistDown = 1000.0f;

    RequiredRaycastHits = 2; // Default to 2 hit required

    bDrawDebug = true;
}

TSubclassOf<UAISense> UAISenseConfig_EnhancedSight::GetSenseImplementation() const
{
    // UE_LOG(LogTemp, Warning, TEXT("UAISenseConfig_EnhancedSight::GetSenseImplementation() called. Returning Implementation: %s"), *GetNameSafe(Implementation));
    return Implementation;
}