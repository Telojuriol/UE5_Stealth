// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyVisualDebugAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h" 
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h" 
#include "CollisionQueryParams.h"
#include "GameFramework/Controller.h"

// ... (El constructor y otras funciones como BeginPlay, OnPossess, OnTargetPerceptionUpdated permanecen igual que en la versión anterior) ...
AMyVisualDebugAIController::AMyVisualDebugAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 1500.0f;
        SightConfig->LoseSightRadius = 2000.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->SetMaxAge(5.0f);
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

        if (AIPerceptionComponent)
        {
            AIPerceptionComponent->ConfigureSense(*SightConfig);
            AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
        }
    }
}

void AMyVisualDebugAIController::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld())
    {
        PlayerPawnPtr = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    }
}

void AMyVisualDebugAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMyVisualDebugAIController::OnTargetPerceptionUpdated);
    }
}

void AMyVisualDebugAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // Lógica de IA o depuración adicional si es necesario
}


void AMyVisualDebugAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* ControlledPawn = GetPawn();
    UWorld* World = GetWorld();

    if (!ControlledPawn || !World) return;

    FVector CurrentViewPoint;
    FRotator CurrentViewRotation;
    AController* PawnController = ControlledPawn->GetController();

    if (ControlledPawn->IsPlayerControlled() && PawnController)
    {
        PawnController->GetPlayerViewPoint(CurrentViewPoint, CurrentViewRotation);
    }
    else
    {
        ControlledPawn->GetActorEyesViewPoint(CurrentViewPoint, CurrentViewRotation);
    }

    const UAISenseConfig_Sight* CurrentSightConfig = SightConfig;

    // --- 1. Dibujar Cono de FOV (Campo de Visión) Simple ---
    if (CurrentSightConfig)
    {
        const FVector FOVOrigin = CurrentViewPoint;
        const FRotator FOVEyeRotation = CurrentViewRotation;
        const FVector FOVForwardVector = FOVEyeRotation.Vector();
        const FVector FOVUpVector = FOVEyeRotation.Quaternion().GetUpVector();
        const float Radius = CurrentSightConfig->SightRadius;
        const float PeripheralVisionAngle = CurrentSightConfig->PeripheralVisionAngleDegrees;

        FVector LeftDir = FOVForwardVector.RotateAngleAxis(-PeripheralVisionAngle / 2.0f, FOVUpVector);
        FVector RightDir = FOVForwardVector.RotateAngleAxis(PeripheralVisionAngle / 2.0f, FOVUpVector);

        FVector LeftEndPoint = FOVOrigin + LeftDir * Radius;
        FVector RightEndPoint = FOVOrigin + RightDir * Radius;

        DrawDebugLine(World, FOVOrigin, LeftEndPoint, VisionConeColor, false, 0.0f, 0, VisionConeLineThickness);
        DrawDebugLine(World, FOVOrigin, RightEndPoint, VisionConeColor, false, 0.0f, 0, VisionConeLineThickness);

        const int32 ArcSegments = 10;
        FVector PrevArcPoint = LeftEndPoint;
        for (int32 i = 1; i <= ArcSegments; ++i)
        {
            float CurrentAngleFraction = static_cast<float>(i) / static_cast<float>(ArcSegments);
            float AngleDegrees = -PeripheralVisionAngle / 2.0f + PeripheralVisionAngle * CurrentAngleFraction;
            FVector ArcDir = FOVForwardVector.RotateAngleAxis(AngleDegrees, FOVUpVector);
            FVector CurrentArcPoint = FOVOrigin + ArcDir * Radius;
            DrawDebugLine(World, PrevArcPoint, CurrentArcPoint, VisionConeColor, false, 0.0f, 0, VisionConeLineThickness);
            PrevArcPoint = CurrentArcPoint;
        }
    }

    // --- 2. Dibujar LineTrace constante al Jugador si está en FOV ---
    APawn* PlayerPawn = PlayerPawnPtr.Get();

    if (PlayerPawn && PlayerPawn->IsValidLowLevel() && !PlayerPawn->IsPendingKillPending())
    {
        if (CurrentSightConfig)
        {
            FVector AILineTraceOrigin_EyeLocation = CurrentViewPoint;
            FRotator AILineTraceOrigin_EyeRotation = CurrentViewRotation;

            FVector PlayerLocation = PlayerPawn->GetActorLocation();
            FVector DirectionToPlayerRaw = PlayerLocation - AILineTraceOrigin_EyeLocation;

            if (DirectionToPlayerRaw.SizeSquared() <= FMath::Square(CurrentSightConfig->SightRadius))
            {
                FVector AIForwardVectorForLoS = AILineTraceOrigin_EyeRotation.Vector();
                FVector DirectionToPlayerNormalized = DirectionToPlayerRaw.GetSafeNormal();
                float DotProduct = FVector::DotProduct(AIForwardVectorForLoS, DirectionToPlayerNormalized);
                DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);
                float AngleToPlayerDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

                if (AngleToPlayerDegrees <= CurrentSightConfig->PeripheralVisionAngleDegrees / 2.0f)
                {
                    FHitResult HitResult;
                    FCollisionQueryParams TraceParams(FName(TEXT("PlayerVisibilityDebugTrace")), true, ControlledPawn);
                    TraceParams.bTraceComplex = false;

                    bool bHit = World->LineTraceSingleByChannel(
                        HitResult,
                        AILineTraceOrigin_EyeLocation,
                        PlayerLocation,
                        ECC_Visibility,
                        TraceParams
                    );

                    if (bHit)
                    {
                        if (HitResult.GetActor() == PlayerPawn)
                        {
                            // El rayo golpeó directamente al jugador
                            DrawDebugLine(World, AILineTraceOrigin_EyeLocation, HitResult.Location, LoSClearColor, false, 0.0f, 0, LoSTraceThickness);
                        }
                        else
                        {
                            // El rayo golpeó un obstáculo.
                            // Dibuja la línea SOLO hasta el punto de impacto del obstáculo.
                            DrawDebugLine(World, AILineTraceOrigin_EyeLocation, HitResult.Location, LoSObstacleColor, false, 0.0f, 0, LoSTraceThickness);
                            // SE HA ELIMINADO LA LÍNEA QUE IBA DESDE EL OBSTÁCULO AL JUGADOR
                        }
                    }
                    else
                    {
                        // El rayo no golpeó nada (camino despejado hacia la ubicación del jugador)
                        DrawDebugLine(World, AILineTraceOrigin_EyeLocation, PlayerLocation, LoSClearColor, false, 0.0f, 0, LoSTraceThickness);
                    }
                }
            }
        }
    }
}