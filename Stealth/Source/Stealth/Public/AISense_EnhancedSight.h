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
};
