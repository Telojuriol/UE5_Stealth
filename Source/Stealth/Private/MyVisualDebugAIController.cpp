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

        if (AIPerceptionComponent) // Asegurarse de que AIPerceptionComponent es válido antes de usarlo
        {
            AIPerceptionComponent->ConfigureSense(*SightConfig);
            AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
        }
    }

    FocusedTarget = nullptr;
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
    if (!Actor) return;

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            // FocusedTarget = Actor; // Comentado o usado para lógica de IA, no para este debug visual
        }
        else
        {
            // if (Actor == FocusedTarget) { FocusedTarget = nullptr; }
        }
    }
}

void AMyVisualDebugAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* ControlledPawn = GetPawn();
    UWorld* World = GetWorld();

    // --- 1. Dibujar Cono de FOV (Campo de Visión) ---
    if (ControlledPawn && SightConfig && World) // AIPerceptionComponent no es estrictamente necesario aquí si GetActorEyesViewPoint es del controller/pawn
    {
        FVector FOVOrigin;
        FRotator FOVEyeRotation;
        // Obtener el punto de vista de los "ojos" para el origen del cono de visión
        this->GetActorEyesViewPoint(FOVOrigin, FOVEyeRotation);
        // Alternativamente, si los ojos están definidos por el Pawn:
        // ControlledPawn->GetActorEyesViewPoint(FOVOrigin, FOVEyeRotation);

        const FVector FOVForwardVector = FOVEyeRotation.Vector(); // Dirección "adelante" de los ojos
        // Usar el vector "arriba" de la rotación de los ojos para que el cono se incline con la mirada.
        // Si quieres que el cono siempre se abra horizontalmente respecto al Pawn, usa: ControlledPawn->GetActorUpVector();
        const FVector FOVUpVector = FOVEyeRotation.Quaternion().GetUpVector();
        const float Radius = SightConfig->SightRadius;

        FVector LeftDir = FOVForwardVector.RotateAngleAxis(-SightConfig->PeripheralVisionAngleDegrees / 2.0f, FOVUpVector);
        FVector RightDir = FOVForwardVector.RotateAngleAxis(SightConfig->PeripheralVisionAngleDegrees / 2.0f, FOVUpVector);

        // Usar FOVOrigin como el punto de inicio de las líneas del cono
        FVector LeftEndPoint = FOVOrigin + LeftDir * Radius;
        FVector RightEndPoint = FOVOrigin + RightDir * Radius;

        DrawDebugLine(World, FOVOrigin, LeftEndPoint, VisionConeColor, false, 0.0f, 0, VisionConeLineThickness);
        DrawDebugLine(World, FOVOrigin, RightEndPoint, VisionConeColor, false, 0.0f, 0, VisionConeLineThickness);

        const int32 ArcSegments = 10;
        FVector PrevArcPoint = LeftEndPoint;
        for (int32 i = 1; i <= ArcSegments; ++i)
        {
            float CurrentAngleFraction = static_cast<float>(i) / static_cast<float>(ArcSegments);
            float AngleDegrees = -SightConfig->PeripheralVisionAngleDegrees / 2.0f + SightConfig->PeripheralVisionAngleDegrees * CurrentAngleFraction;
            FVector ArcDir = FOVForwardVector.RotateAngleAxis(AngleDegrees, FOVUpVector);
            FVector CurrentArcPoint = FOVOrigin + ArcDir * Radius; // El arco también se origina desde FOVOrigin
            DrawDebugLine(World, PrevArcPoint, CurrentArcPoint, VisionConeColor, false, 0.0f, 0, VisionConeLineThickness);
            PrevArcPoint = CurrentArcPoint;
        }
    }

    // --- 2. Dibujar LineTrace constante al Jugador si está en FOV ---
    APawn* PlayerPawn = PlayerPawnPtr.Get();

    // Usamos IsPendingKillPending() que está en Actor.h y debería compilar bien para AActor y APawn.
    if (PlayerPawn && PlayerPawn->IsValidLowLevel() && !PlayerPawn->IsPendingKillPending())
    {
        // Asegurarse de que los componentes necesarios para la IA y el mundo son válidos
        if (ControlledPawn && SightConfig && World) // AIPerceptionComponent no es estrictamente necesario para this->GetActorEyesViewPoint
        {
            FVector AILineTraceOrigin_EyeLocation;
            FRotator AILineTraceOrigin_EyeRotation;
            this->GetActorEyesViewPoint(AILineTraceOrigin_EyeLocation, AILineTraceOrigin_EyeRotation);

            FVector PlayerLocation = PlayerPawn->GetActorLocation();
            FVector DirectionToPlayerRaw = PlayerLocation - AILineTraceOrigin_EyeLocation;

            if (DirectionToPlayerRaw.SizeSquared() <= FMath::Square(SightConfig->SightRadius))
            {
                FVector AIForwardVectorForLoS = AILineTraceOrigin_EyeRotation.Vector();
                FVector DirectionToPlayerNormalized = DirectionToPlayerRaw.GetSafeNormal();
                float DotProduct = FVector::DotProduct(AIForwardVectorForLoS, DirectionToPlayerNormalized);
                DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);
                float AngleToPlayerDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

                if (AngleToPlayerDegrees <= SightConfig->PeripheralVisionAngleDegrees / 2.0f)
                {
                    FHitResult HitResult;
                    FCollisionQueryParams TraceParams(FName(TEXT("PlayerVisibilityTrace")), true, ControlledPawn);
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
                            DrawDebugLine(World, AILineTraceOrigin_EyeLocation, HitResult.Location, LoSClearColor, false, 0.0f, 0, LoSTraceThickness);
                        }
                        else
                        {
                            DrawDebugLine(World, AILineTraceOrigin_EyeLocation, HitResult.Location, LoSObstacleColor, false, 0.0f, 0, LoSTraceThickness);
                            DrawDebugLine(World, HitResult.Location, PlayerLocation, LoSOccludedPathColor, false, 0.0f, 0, LoSTraceThickness * 0.7f);
                        }
                    }
                    else
                    {
                        DrawDebugLine(World, AILineTraceOrigin_EyeLocation, PlayerLocation, LoSClearColor, false, 0.0f, 0, LoSTraceThickness);
                    }
                }
            }
        }
    }
}