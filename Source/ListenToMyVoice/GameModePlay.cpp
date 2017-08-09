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

void AGameModePlay::PostLogin(APlayerController* NewPlayer) {
    Super::PostLogin(NewPlayer);
    ULibraryUtils::Log("AGameModePlay::PostLogin");
}

//bool AGameModePlay::SERVER_RespawnPlayer_Validate(APlayerControllerPlay* PlayerController,
//                                                  FPlayerInfo info) {
//    return true;
//}
//
//void AGameModePlay::SERVER_RespawnPlayer_Implementation(APlayerControllerPlay* PlayerController,
//                                                        FPlayerInfo info) {
//    if (PlayerController->GetPawn()) PlayerController->GetPawn()->Destroy();
//
//    FTransform transform = FindPlayerStart(PlayerController, info.Name)->GetActorTransform();
//    APawn* actor = Cast<APawn>(GetWorld()->SpawnActor(info.CharacterClass, &transform));
//    if (actor) {
//        PlayerController->Possess(actor);
//        PlayerController->AfterPossessed();
//
//        if (!_HostController) _HostController = PlayerController;
//        else _GuestController = PlayerController;
//    }
//}

bool AGameModePlay::SERVER_PlayerDead_Validate(AController* PlayerController) {
    return true;
}

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