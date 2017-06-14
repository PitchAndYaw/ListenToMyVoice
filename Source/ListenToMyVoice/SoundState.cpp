// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "SoundState.h"


USoundState::USoundState() {
    PrimaryComponentTick.bCanEverTick = true;
    bAutoActivate = false;
}

void USoundState::BeginPlay() {
    Super::BeginPlay();
}

int USoundState::SwitchState_Implementation() {
	if (!_onPress) {
		Play();
	}
    return 0;
}