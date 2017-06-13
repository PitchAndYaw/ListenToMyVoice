// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerStatePlay.h"


void APlayerStatePlay::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APlayerStatePlay, _IsTalking);
}

APlayerStatePlay::APlayerStatePlay(const class FObjectInitializer& OI) : Super(OI) {
    bReplicates = true;

    _IsTalking = false;
}

bool APlayerStatePlay::SetIsTalking_Validate(const bool IsTalking) {
    return true;
}
void APlayerStatePlay::SetIsTalking_Implementation(const bool IsTalking) {
    _IsTalking = IsTalking;
    ForceNetUpdate();
}

bool APlayerStatePlay::GetIsTalking() {
    return _IsTalking;
}