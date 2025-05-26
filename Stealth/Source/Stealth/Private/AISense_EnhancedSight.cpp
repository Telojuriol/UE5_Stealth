// AISense_EnhancedSight.cpp

#include "AISense_EnhancedSight.h"
#include "Perception/AIPerceptionComponent.h" // Para UAIPerceptionComponent (necesario para GetSenseConfig y FPerceptionListener)
#include "Perception/AIPerceptionTypes.h"   // Para FAIStimulus, FPerceptionListener, etc.
#include "AISenseConfig_EnhancedSight.h"    // Para Cast a la config específica
#include "EngineUtils.h"                    // Para TActorIterator
#include "GameFramework/Pawn.h"             // Para APawn

UAISense_EnhancedSight::UAISense_EnhancedSight()
{
    UE_LOG(LogTemp, Warning, TEXT("EnhancedSight Constructor() called"));
    RequestImmediateUpdate();
}

void UAISense_EnhancedSight::PostInitProperties()
{
    Super::PostInitProperties();
    UE_LOG(LogTemp, Warning, TEXT("UAISense_EnhancedSight::PostInitProperties() CALLED"));
}

float UAISense_EnhancedSight::Update()
{
    UE_LOG(LogTemp, Warning, TEXT("UAISense_EnhancedSight::Update() --- LLAMADO ---"));
    ProcessSight();
    return 0.5f;
}

void UAISense_EnhancedSight::ProcessSight()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    AIPerception::FListenerMap& ListenersMap = *GetListeners();
    for (AIPerception::FListenerMap::TIterator ListenerIt(ListenersMap); ListenerIt; ++ListenerIt)
    {
        FPerceptionListener& Listener = ListenerIt->Value; // Listener es FPerceptionListener&
        const AActor* ListenerActor = Listener.GetBodyActor();
        UAIPerceptionComponent* PerceptionComponent = Listener.Listener.Get(); // Obtener el componente de percepción

        if (!ListenerActor || !PerceptionComponent) // Necesitamos el PerceptionComponent para GetSenseConfig
        {
            continue;
        }

        // CORRECCIÓN: Usar PerceptionComponent->GetSenseConfig()
        const UAISenseConfig* FoundConfig = PerceptionComponent->GetSenseConfig(GetSenseID());
        const UAISenseConfig_EnhancedSight* MyConfig = Cast<const UAISenseConfig_EnhancedSight>(FoundConfig);

        if (!MyConfig)
        {
            // UE_LOG(LogTemp, Warning, TEXT("EnhancedSightConfig no encontrada para el listener %s"), *ListenerActor->GetName());
            continue;
        }

        const float CurrentSightRadius = MyConfig->SightRadius;
        const float CurrentPeripheralVisionAngleDegrees = MyConfig->PeripheralVisionAngle;
        const float PeripheralVisionAngleDotProduct = FMath::Cos(FMath::DegreesToRadians(CurrentPeripheralVisionAngleDegrees * 0.5f));

        FVector ListenerLocation = ListenerActor->GetActorLocation();
        FVector ListenerForward = ListenerActor->GetActorForwardVector();
        FVector ViewPoint = ListenerLocation;

        APawn* ListenerPawn = Cast<APawn>(const_cast<AActor*>(ListenerActor));
        if (ListenerPawn)
        {
            FRotator ViewRotation;
            ListenerPawn->GetActorEyesViewPoint(ViewPoint, ViewRotation);
            ListenerForward = ViewRotation.Vector();
        }

        for (TActorIterator<APawn> TargetIt(World); TargetIt; ++TargetIt)
        {
            APawn* TargetActor = *TargetIt;

            if (!TargetActor || TargetActor == ListenerActor)
            {
                continue;
            }

            FVector TargetLocation = TargetActor->GetActorLocation();
            FVector DirectionToTarget = TargetLocation - ViewPoint;
            const float DistanceToTarget = DirectionToTarget.Size();

            if (DistanceToTarget > CurrentSightRadius)
            {
                continue;
            }

            if (DistanceToTarget <= UE_KINDA_SMALL_NUMBER)
            {
                continue;
            }
            DirectionToTarget.Normalize();

            if (FVector::DotProduct(ListenerForward, DirectionToTarget) < PeripheralVisionAngleDotProduct)
            {
                continue;
            }

            FHitResult HitResult;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(ListenerActor);

            bool bHasLineOfSight = false;
            if (!World->LineTraceSingleByChannel(
                HitResult,
                ViewPoint,
                TargetLocation,
                ECC_Visibility,
                QueryParams
            ))
            {
                bHasLineOfSight = true;
            }
            else
            {
                if (HitResult.GetActor() == TargetActor)
                {
                    bHasLineOfSight = true;
                }
            }

            if (bHasLineOfSight)
            {
                UE_LOG(LogTemp, Warning, TEXT("IA (%s) VE a (%s). Distancia: %.2f"), *ListenerActor->GetName(), *TargetActor->GetName(), DistanceToTarget);

                // CORRECCIÓN: Usar el constructor de FAIStimulus de 6 argumentos de tu AIPerceptionTypes.h
                // y FAIStimulus::SensingSucceeded para el FResult.
                FAIStimulus Stimulus(
                    *this,                                // const UAISense& Sense
                    1.0f,                                 // float Strength
                    TargetLocation,                       // const FVector& InStimulusLocation
                    ViewPoint,                            // const FVector& InReceiverLocation
                    FAIStimulus::SensingSucceeded,        // FAIStimulus::FResult Result
                    FName()                               // FName InStimulusTag (FName() crea un NAME_None)
                );

                // CORRECCIÓN: Registrar el estímulo directamente con el Listener,
                // ya que FPerceptionListener::RegisterStimulus es pública en tu versión.
                // TargetActor es el "Source" del estímulo visual.
                Listener.RegisterStimulus(TargetActor, Stimulus);
            }
        }
    }
}