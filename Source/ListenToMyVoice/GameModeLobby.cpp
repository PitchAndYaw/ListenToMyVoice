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

    APlayerControllerLobby* PC = Cast<APlayerControllerLobby>(NewPlayer);
    if(PC) PC->CLIENT_Init();
}

bool AGameModeLobby::SERVER_SpawnCharacter_Validate(bool IsVR, APlayerController* NewPlayer) {
    return true; 
}
void AGameModeLobby::SERVER_SpawnCharacter_Implementation(bool IsVR, APlayerController* NewPlayer) {
    APlayerControllerLobby* PC = Cast<APlayerControllerLobby>(NewPlayer);
    if (HasAuthority() && PC) {
        /* SPAWN PLAYER */
        if (PC->GetPawn()) PC->GetPawn()->Destroy();

        APlayerStatePlay* PS = Cast<APlayerStatePlay>(PC->PlayerState);
        UNWGameInstance* GI = Cast<UNWGameInstance>(GetGameInstance());
        if (PS && GI) {
            if (NumPlayers == 1) {
                PS->SetPlayerName("host");
                PS->SERVER_SetCharacterClass(IsVR ? GI->_VRBoyClass : GI->_BoyClass);
            }
            else {
                PS->SetPlayerName("guest");
                PS->SERVER_SetCharacterClass(IsVR ? GI->_VRGirlClass : GI->_GirlClass);
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