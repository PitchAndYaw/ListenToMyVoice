// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "GameModeLobby.h"

#include "NWGameInstance.h"
#include "GameSessionDedicated.h"
#include "PlayerControllerLobby.h"
#include "GameStatePlay.h"
#include "PlayerStatePlay.h"


void AGameModeLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGameModeLobby, _MapNameGM);
}

AGameModeLobby::AGameModeLobby(const class FObjectInitializer& OI) : Super(OI) {
    DefaultPawnClass = nullptr;
    PlayerControllerClass = APlayerControllerLobby::StaticClass();
    GameStateClass = AGameStatePlay::StaticClass();
    PlayerStateClass = APlayerStatePlay::StaticClass();
    GameSessionClass = AGameSessionDedicated::StaticClass();

    bUseSeamlessTravel = true;

    _MapNameGM = USettings::Get()->LevelToPlay.GetLongPackageName();
}

void AGameModeLobby::InitGame(const FString & MapName, const FString & Options,
                              FString & ErrorMessage) {
    Super::InitGame(MapName, Options, ErrorMessage);

    if (GameSession) GameSession->bRequiresPushToTalk = false;
}

void AGameModeLobby::PostLogin(APlayerController* NewPlayer) {
    Super::PostLogin(NewPlayer);
    ULibraryUtils::Log("AGameModeLobby::PostLogin");

    APlayerControllerLobby* PC = Cast<APlayerControllerLobby>(NewPlayer);
    UNWGameInstance* GI = Cast<UNWGameInstance>(GetGameInstance());
    if (HasAuthority() && PC && GI) {
        /* SPAWN PLAYER */
        ULibraryUtils::Log("SPAWN PLAYER");
        if (PC->GetPawn()) PC->GetPawn()->Destroy();

        APlayerStatePlay* PS = Cast<APlayerStatePlay>(PC->PlayerState);
        if (PS) {
            PS->SetIsVR(PC->_IsVR);

            ULibraryUtils::Log(FString::Printf(TEXT("NumPlayers=%i"), NumPlayers));
            if (NumPlayers == 1) {
                PS->SetPlayerName("host");
                PS->SetCharacterClass(PC->_IsVR ? GI->_VRBoyClass : GI->_BoyClass);
            }
            else {
                PS->SetPlayerName("guest");
                PS->SetCharacterClass(PC->_IsVR ? GI->_VRGirlClass : GI->_GirlClass);
            }

            FTransform Transform = FindPlayerStart(PC, PS->PlayerName)->GetActorTransform();
            APawn* Actor = Cast<APawn>(GetWorld()->SpawnActor(PS->_CharacterClass, &Transform));
            if (Actor) PC->Possess(Actor);
        }
    }
}

void AGameModeLobby::LaunchGame() {
    GetWorld()->ServerTravel(_MapNameGM, true);
}