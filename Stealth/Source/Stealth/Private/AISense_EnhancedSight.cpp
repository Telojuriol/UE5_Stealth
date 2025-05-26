#include "AISense_EnhancedSight.h"
#include "Perception/AIPerceptionSystem.h"

UAISense_EnhancedSight::UAISense_EnhancedSight()
{
    UE_LOG(LogTemp, Warning, TEXT("EnhancedSight Contructor() called"));
    RequestImmediateUpdate();
}

void UAISense_EnhancedSight::PostInitProperties()
{
    Super::PostInitProperties(); // ¡MUY IMPORTANTE llamar a la versión de la clase padre!
    UE_LOG(LogTemp, Warning, TEXT("UAISense_EnhancedSight::PostInitProperties() CALLED"));
}

float UAISense_EnhancedSight::Update()
{
    UE_LOG(LogTemp, Warning, TEXT("EnhancedSight Update() called"));
    //ProcessSight();
    // Devuelve un valor para el tiempo hasta el próximo Update. Retorna 0 si es constante.
    return 0.5f;
}

/*void UAISense_EnhancedSight::ProcessSight()
{  
   UWorld* World = GetWorld();  
   if (!World)  
       return;  

   const float LogDistanceThreshold = 1000.0f;  

   AIPerception::FListenerMap& ListenersMap = *GetListeners();  

   // Iterate through all the listeners that have the Vibration sense. Listeners are actors with an AIPerception component.  
   for (AIPerception::FListenerMap::TIterator ListenerIt(ListenersMap); ListenerIt; ++ListenerIt)  
   {  
       FPerceptionListener& Listener = ListenerIt->Value;  
       const AActor* ListenerActor = Listener.GetBodyActor();
       if (!ListenerActor || !Listener.HasSense(GetSenseID()))  
       {  
           // Skip listeners not interested in this sense  
           continue;  
       }  

       float Distance = FVector::Dist(ListenerActor->GetActorLocation(), ListenerActor->GetActorLocation());  

       if (Distance < LogDistanceThreshold)  
       {  
           UE_LOG(LogTemp, Warning, TEXT("Player is within %.2f units of %s"), Distance, *ListenerActor->GetName());  
       }  
   }  
}*/

