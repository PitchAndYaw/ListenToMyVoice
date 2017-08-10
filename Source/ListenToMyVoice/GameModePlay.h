// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "GameModePlay.generated.h"


UCLASS()
class LISTENTOMYVOICE_API AGameModePlay : public AGameMode {
    GENERATED_BODY()

public:
    AGameModePlay(const class FObjectInitializer& OI);

    virtual void InitGame(const FString & MapName, const FString & Options,
                          FString & ErrorMessage) override;

    virtual void HandleSeamlessTravelPlayer(AController*& C);

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_PlayerDead(AController* Controller);

protected:
    class APlayerControllerPlay* _HostController;
    class APlayerControllerPlay* _GuestController;
};
