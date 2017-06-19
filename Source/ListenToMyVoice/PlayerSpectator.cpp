// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerSpectator.h"

#include "MenuInteraction.h"

APlayerSpectator::APlayerSpectator(const FObjectInitializer& OI) : Super(OI) {
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;

    _PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
    _MenuInteractionComp = CreateDefaultSubobject<UMenuInteraction>(TEXT("Menu Interaction"));

    _PlayerCamera->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    _PlayerCamera->bUsePawnControlRotation = true;
    _MenuInteractionComp->AttachToComponent(_PlayerCamera, FAttachmentTransformRules::KeepRelativeTransform);
    _MenuInteractionComp->_RayParameter = 100000;

    USpectatorPawnMovement* Movement = Cast<USpectatorPawnMovement>(GetMovementComponent());
    if (Movement) Movement->MaxSpeed = 10;
}

void APlayerSpectator::BeginPlay() {
    Super::BeginPlay();
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