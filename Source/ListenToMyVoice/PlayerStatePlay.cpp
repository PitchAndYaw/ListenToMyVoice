// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerStatePlay.h"


void APlayerStatePlay::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APlayerStatePlay, _IsTalking);
    DOREPLIFETIME(APlayerStatePlay, _CharacterClass);
}

APlayerStatePlay::APlayerStatePlay(const class FObjectInitializer& OI) : Super(OI) {
    bReplicates = true;

    _IsTalking = false;
}

void APlayerStatePlay::OverrideWith(APlayerState* PlayerState) {
    Super::OverrideWith(PlayerState);

    APlayerStatePlay* PSP = Cast<APlayerStatePlay>(PlayerState);
    if (PSP) {
        _CharacterClass = PSP->_CharacterClass;
    }
}


void APlayerStatePlay::CopyProperties(APlayerState* PlayerState) {
    Super::CopyProperties(PlayerState);

    APlayerStatePlay* PSP = Cast<APlayerStatePlay>(PlayerState);
    if (PSP) {
        PSP->_CharacterClass = _CharacterClass;
    }
}

/******************************** GETTERS/SETTERS ************************************************/
bool APlayerStatePlay::SERVER_SetIsTalking_Validate(const bool IsTalking) {
    return true;
}
void APlayerStatePlay::SERVER_SetIsTalking_Implementation(const bool IsTalking) {
    _IsTalking = IsTalking;
    ForceNetUpdate();
}

bool APlayerStatePlay::GetIsTalking() {
    return _IsTalking;
}

bool APlayerStatePlay::SERVER_SetCharacterClass_Validate(TSubclassOf<ACharacter> CharacterClass) {
    return true;
}
void APlayerStatePlay::SERVER_SetCharacterClass_Implementation(TSubclassOf<ACharacter> CharacterClass) {
    _CharacterClass = CharacterClass;
    ForceNetUpdate();
}

TSubclassOf<ACharacter> APlayerStatePlay::GetCharacterClass() {
    return _CharacterClass;
}