// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerStatePlay.h"


void APlayerStatePlay::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APlayerStatePlay, _IsTalking);
    DOREPLIFETIME(APlayerStatePlay, _IsVR);
}

APlayerStatePlay::APlayerStatePlay(const class FObjectInitializer& OI) : Super(OI) {
    bReplicates = true;

    _IsTalking = false;
    _IsVR = false;
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


bool APlayerStatePlay::SetIsVR_Validate(const bool IsVR) {
    return true;
}
void APlayerStatePlay::SetIsVR_Implementation(const bool IsVR) {
    _IsVR = IsVR;
    ForceNetUpdate();
}

bool APlayerStatePlay::GetIsVR() {
    return _IsVR;
}


bool APlayerStatePlay::SetCharacterClass_Validate(TSubclassOf<ACharacter> CharacterClass) {
    return true;
}
void APlayerStatePlay::SetCharacterClass_Implementation(TSubclassOf<ACharacter> CharacterClass) {
    _CharacterClass = CharacterClass;
    ForceNetUpdate();
}

TSubclassOf<ACharacter> APlayerStatePlay::GetCharacterClass() {
    return _CharacterClass;
}