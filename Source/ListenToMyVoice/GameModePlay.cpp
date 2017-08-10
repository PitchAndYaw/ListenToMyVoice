// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "GameModePlay.h"

#include "PlayerControllerPlay.h"
#include "GameStatePlay.h"
#include "PlayerStatePlay.h"
#include "PlayerSpectator.h"
#include "GameSessionDedicated.h"


AGameModePlay::AGameModePlay(const class FObjectInitializer& OI) : Super(OI) {
    DefaultPawnClass = nullptr;
    SpectatorClass = APlayerSpectator::StaticClass();
    PlayerControllerClass = APlayerControllerPlay::StaticClass();
    GameStateClass = AGameStatePlay::StaticClass();
    PlayerStateClass = APlayerStatePlay::StaticClass();
    GameSessionClass = AGameSessionDedicated::StaticClass();

    bUseSeamlessTravel = true;
}

void AGameModePlay::InitGame(const FString & MapName, const FString & Options,
                             FString & ErrorMessage) {
    Super::InitGame(MapName, Options, ErrorMessage);

    if (GameSession) GameSession->bRequiresPushToTalk = true;
}

void AGameModePlay::HandleSeamlessTravelPlayer(AController*& C) {
    Super::HandleSeamlessTravelPlayer(C);

    ULibraryUtils::Log("AGameModePlay::HandleSeamlessTravelPlayer");
    APlayerControllerPlay* PC = Cast<APlayerControllerPlay>(C);
    if (PC) {
        ULibraryUtils::Log(FString::Printf(TEXT("AGameModePlay::HandleSeamlessTravelPlayer - PC: %s"), *PC->GetFName().ToString()));

        APlayerStatePlay* PS = Cast<APlayerStatePlay>(PC->PlayerState);
        if (PS) {
            ULibraryUtils::Log(FString::Printf(TEXT("Player Name: %s"), *PS->PlayerName));
            ULibraryUtils::Log(FString::Printf(TEXT("Player Pawn: %s"), *PS->_CharacterClass));

            FTransform Transform = FindPlayerStart(PC, PS->PlayerName)->GetActorTransform();
            APawn* Actor = Cast<APawn>(GetWorld()->SpawnActor(PS->_CharacterClass, &Transform));
            if (Actor) {
                PC->Possess(Actor);
                if (!_HostController) _HostController = PC;
                else _GuestController = PC;
            }
        }
    }
}

bool AGameModePlay::SERVER_PlayerDead_Validate(AController* Controller) { return true; }
void AGameModePlay::SERVER_PlayerDead_Implementation(AController* Controller) {
    APlayerControllerPlay* PlayerController = Cast<APlayerControllerPlay>(Controller);
    if (PlayerController) {
        PlayerController->ChangeState(NAME_Spectating);

        if (_HostController == PlayerController) {
            _HostController->CLIENT_Dead();
            if(_GuestController) _GuestController->CLIENT_ShowMenu();
        }
        if (_GuestController == PlayerController) {
            _GuestController->CLIENT_GotoState(NAME_Spectating);
            _HostController->CLIENT_ShowMenu();
        }
    }
}