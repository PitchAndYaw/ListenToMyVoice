// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "GameStatePlay.h"

#include "GameModePlay.h"
#include "SwitcherComponent.h"


void AGameStatePlay::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGameStatePlay, _LobbyWallOn);
}

AGameStatePlay::AGameStatePlay(const class FObjectInitializer& OI) : Super(OI){
    bReplicates = true;

    _LobbyWallOn = false;
}

bool AGameStatePlay::ActivateWall_Validate(AActor* WallActor) { return true; }
void AGameStatePlay::ActivateWall_Implementation(AActor* WallActor) {
    ULibraryUtils::Log("ActivateWall_Implementation");
    if (!_LobbyWallOn) {
        _LobbyWallOn = true;

        USwitcherComponent* Switcher = Cast<USwitcherComponent>(WallActor->GetOwner()->
                                                               GetComponentByClass(USwitcherComponent::StaticClass()));

        if (Switcher) {
            Switcher->ActivateSwitcher();
        }

        ForceNetUpdate();
    }
}