// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "GameStatePlay.generated.h"


UCLASS()
class LISTENTOMYVOICE_API AGameStatePlay : public AGameState {
    GENERATED_BODY()

public:
    UPROPERTY(Replicated)
    bool _LobbyWallOn;

    AGameStatePlay(const class FObjectInitializer& OI);

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
    void ActivateWall(AActor* WallActor);
};
