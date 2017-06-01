// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerControllerLobby.h"

#include "GameFramework/GameMode.h"
#include "GameModeLobby.generated.h"


UCLASS()
class LISTENTOMYVOICE_API AGameModeLobby : public AGameMode {
    GENERATED_BODY()

public:
    AGameModeLobby(const class FObjectInitializer& OI);
    virtual void PostLogin(APlayerController * NewPlayer) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_SwapCharacter(APlayerControllerLobby* PlayerController, FPlayerInfo info,
                              bool ChangeStatus);

    UFUNCTION(BlueprintCallable, Category = "Launch Game")
    void LaunchGame();

    virtual void InitGame(const FString & MapName, const FString & Options, 
                          FString & ErrorMessage) override;

protected:
    UPROPERTY(Replicated)
    FName _ServerName;
    UPROPERTY(Replicated)
    FString _MapNameGM;

    UPROPERTY(Replicated)
    int32 _MaxPlayers;
};