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
    virtual void BeginPlay() override; // A�adido para inicializar PlayerPawnPtr
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAISenseConfig_Sight* SightConfig;

    // Ya no necesitamos OnTargetPerceptionUpdated para esta l�gica de debug espec�fica del jugador,
    // pero la dejamos por si la IA la usa para otras cosas.
    // La variable FocusedTarget tambi�n se puede quitar si ya no se usa.
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
    // Constantes para el cono de visi�n
    const FColor VisionConeColor = FColor::Cyan;
    const float VisionConeLineThickness = 2.0f;

    // Constantes para el LineTrace de L�nea de Visi�n (LoS)
    const FColor LoSObstacleColor = FColor::Red;
    const FColor LoSClearColor = FColor::Green;
    const FColor LoSOccludedPathColor = FColor::Orange;
    const float LoSTraceThickness = 2.5f; // La duraci�n ser� 0.0f (se dibuja en Tick)

    // Referencia al pe�n del jugador
    TWeakObjectPtr<APawn> PlayerPawnPtr;

    // El FocusedTarget anterior, puedes decidir si a�n lo necesitas para otra l�gica.
    // Para el requerimiento actual de trazar siempre al jugador en FOV, no se usa.
    UPROPERTY(Transient)
    AActor* FocusedTarget;
};