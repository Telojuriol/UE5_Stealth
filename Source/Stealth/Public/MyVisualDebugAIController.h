// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/Pawn.h" // Para APawn
#include "MyVisualDebugAIController.generated.h"

class AActor;
class UAISenseConfig_Sight;
struct FAIStimulus;

UCLASS()
class STEALTH_API AMyVisualDebugAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMyVisualDebugAIController();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override; // Añadido para inicializar PlayerPawnPtr
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAISenseConfig_Sight* SightConfig;

    // Ya no necesitamos OnTargetPerceptionUpdated para esta lógica de debug específica del jugador,
    // pero la dejamos por si la IA la usa para otras cosas.
    // La variable FocusedTarget también se puede quitar si ya no se usa.
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
    // Constantes para el cono de visión
    const FColor VisionConeColor = FColor::Cyan;
    const float VisionConeLineThickness = 2.0f;

    // Constantes para el LineTrace de Línea de Visión (LoS)
    const FColor LoSObstacleColor = FColor::Red;
    const FColor LoSClearColor = FColor::Green;
    const FColor LoSOccludedPathColor = FColor::Orange;
    const float LoSTraceThickness = 2.5f; // La duración será 0.0f (se dibuja en Tick)

    // Referencia al peón del jugador
    TWeakObjectPtr<APawn> PlayerPawnPtr;

    // El FocusedTarget anterior, puedes decidir si aún lo necesitas para otra lógica.
    // Para el requerimiento actual de trazar siempre al jugador en FOV, no se usa.
    UPROPERTY(Transient)
    AActor* FocusedTarget;
};