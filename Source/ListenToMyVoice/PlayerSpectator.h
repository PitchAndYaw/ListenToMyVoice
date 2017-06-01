// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "PlayerSpectator.generated.h"


UCLASS()
class LISTENTOMYVOICE_API APlayerSpectator : public ASpectatorPawn {
    GENERATED_BODY()

public:
    APlayerSpectator(const FObjectInitializer& OI);

    void ToggleMenuInteraction(bool Activate);
    void ClickLeftPressed();
    void ClickLeftReleased();

protected:
    UPROPERTY(Category = Spectator, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* _PlayerCamera;
    UPROPERTY(Category = Spectator, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UMenuInteraction* _MenuInteractionComp;

    virtual void BeginPlay() override;

public:
    FORCEINLINE UCameraComponent* APlayerSpectator::GetPlayerCamera() const { return _PlayerCamera; }
    FORCEINLINE UMenuInteraction* APlayerSpectator::GetMenuInteractionComp() const { return _MenuInteractionComp; }
};
