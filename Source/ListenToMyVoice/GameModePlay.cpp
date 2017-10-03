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

void AGameModePlay::HandleSeamlessTravelPlayer(AController*& Controller) {
    Super::HandleSeamlessTravelPlayer(Controller);

    APlayerControllerPlay* PC = Cast<APlayerControllerPlay>(Controller);
    if (PC) {
        APlayerStatePlay* PS = Cast<APlayerStatePlay>(PC->PlayerState);
        if (PS) {
            FTransform Transform = FindPlayerStart(PC, PS->PlayerName)->GetActorTransform();
            APawn* Actor = Cast<APawn>(GetWorld()->SpawnActor(PS->_CharacterClass, &Transform));
            if (Actor) PC->Possess(Actor);
        }
    }
}

bool AGameModePlay::SERVER_PlayerDead_Validate(AController* Controller) { return true; }
void AGameModePlay::SERVER_PlayerDead_Implementation(AController* Controller) {
    APlayerControllerPlay* PC = Cast<APlayerControllerPlay>(Controller);
    if (PC) {
        APlayerStatePlay* PSAux;
        APlayerControllerPlay* PCAux;
        APawn* Actor;
        for (FConstPlayerControllerIterator I = GetWorld()->GetPlayerControllerIterator(); I; ++I) {
            PCAux = Cast<APlayerControllerPlay>(I->Get());
            PSAux = PCAux ? Cast<APlayerStatePlay>(PCAux->PlayerState) : nullptr;
            if (PSAux) {
                Actor = PCAux->GetPawn();
                PCAux->UnPossess();
                if (Actor) Actor->Destroy();
                FTransform Transform = FindPlayerStart(PC, PSAux->PlayerName)->GetActorTransform();
                Actor = Cast<APawn>(GetWorld()->SpawnActor(SpectatorClass, &Transform));
                if (Actor) PCAux->Possess(Actor);
            }
        }
    }
}
