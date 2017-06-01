// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "TextureState.h"


// Sets default values for this component's properties
UTextureState::UTextureState() {
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTextureState::BeginPlay() {
    Super::BeginPlay();
}

int UTextureState::SwitchState_Implementation() {
    UTextRenderComponent* textureComp = Cast<UTextRenderComponent>(GetOwner()->GetComponentByClass(
        UTextRenderComponent::StaticClass()));
    if (textureComp) {
        if (textureComp->bVisible == false) {
            textureComp->SetVisibility(true);
        }
        else {
            textureComp->SetVisibility(false);
        }
    }

    return 0;
}
