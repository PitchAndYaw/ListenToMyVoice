// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "SoundState.h"


USoundState::USoundState() {
    PrimaryComponentTick.bCanEverTick = true;
    bAutoActivate = false;
    
    _Parameter = 0;
}

void USoundState::BeginPlay() {
    Super::BeginPlay();
}

int USoundState::SwitchState_Implementation() {
	if (_Parameter == 0) {
		Play();
        _Parameter = 1;
	}
    else {
        Play();
        _Parameter = 0;
    }
    return 0;
}