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
	TArray<UActorComponent*> TextArray = GetOwner()->GetComponentsByClass(UTextRenderComponent::StaticClass());

    UTextRenderComponent* textureComp = Cast<UTextRenderComponent>(GetOwner()->GetComponentByClass(
        UTextRenderComponent::StaticClass()));

	UTextRenderComponent* TextComponent;

	for (UActorComponent* Component : TextArray)
	{
		TextComponent = Cast<UTextRenderComponent>(Component);

		if (TextComponent->bVisible == false) {
			TextComponent->SetVisibility(true);
		}
		else {
			TextComponent->SetVisibility(false);
		}
	}
	
    return 0;
}
