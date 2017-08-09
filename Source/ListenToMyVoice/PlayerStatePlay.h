// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "PlayerStatePlay.generated.h"


UCLASS()
class LISTENTOMYVOICE_API APlayerStatePlay : public APlayerState {
    GENERATED_BODY()

public:
    UPROPERTY(Replicated)
    bool _IsTalking;

    UPROPERTY(Replicated)
    bool _IsVR;

    UPROPERTY(Replicated)
    TSubclassOf<ACharacter> _CharacterClass;

    APlayerStatePlay(const class FObjectInitializer& OI);

    UFUNCTION(Server, Reliable, WithValidation)
    void SetIsTalking(const bool IsTalking);
    bool GetIsTalking();

    UFUNCTION(Server, Reliable, WithValidation)
    void SetIsVR(const bool IsVR);
    bool GetIsVR();

    UFUNCTION(Server, Reliable, WithValidation)
    void SetCharacterClass(TSubclassOf<ACharacter> CharacterClass);
    TSubclassOf<ACharacter> GetCharacterClass();
};
