#include "ListenToMyVoice.h"
#include "PlayerSpectator.h"

#include "MenuInteraction.h"

APlayerSpectator::APlayerSpectator(const FObjectInitializer& OI) : Super(OI) {
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;

    _PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
    _PlayerCamera->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    _PlayerCamera->bUsePawnControlRotation = true;

    _MenuInteractionComp = CreateDefaultSubobject<UMenuInteraction>(TEXT("Menu Interaction"));
    _MenuInteractionComp->_RayParameter = 100000;
    _MenuInteractionComp->AttachToComponent(_PlayerCamera, FAttachmentTransformRules::KeepRelativeTransform);

    USpectatorPawnMovement* Movement = Cast<USpectatorPawnMovement>(GetMovementComponent());
    if (Movement) Movement->MaxSpeed = 10;

    _PlayerCamera->PostProcessBlendWeight = 0.6f;
    _PlayerCamera->PostProcessSettings.bOverride_SceneColorTint = true;
    _PlayerCamera->PostProcessSettings.SceneColorTint = FLinearColor(1, 0, 0, 0.8);
}

void APlayerSpectator::AfterPossessed() {
    ToggleMenuInteraction(false);
}

void APlayerSpectator::ToggleMenuInteraction(bool Activate) {
    _MenuInteractionComp->SetActive(Activate);
    _MenuInteractionComp->SetHiddenInGame(!Activate, true);
    _MenuInteractionComp->SetComponentTickEnabled(Activate);
    _MenuInteractionComp->SetVisibility(Activate, true);
}

/******** USE ITEM LEFT *********/
void APlayerSpectator::ClickLeftPressed() {
    _MenuInteractionComp->PressPointer();
}

void APlayerSpectator::ClickLeftReleased() {
    _MenuInteractionComp->ReleasePointer();
}