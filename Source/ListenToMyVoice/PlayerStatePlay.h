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
    TSubclassOf<ACharacter> _CharacterClass;

    APlayerStatePlay(const class FObjectInitializer& OI);
    void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_SetIsTalking(const bool IsTalking);
    bool GetIsTalking();

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_SetCharacterClass(TSubclassOf<ACharacter> CharacterClass);
    TSubclassOf<ACharacter> GetCharacterClass();

protected:
    void OverrideWith(APlayerState* PlayerState) override;
    void CopyProperties(APlayerState* PlayerState) override;
};
