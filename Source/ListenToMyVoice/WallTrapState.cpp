// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "WallTrapState.h"


UWallTrapState::UWallTrapState() {
    PrimaryComponentTick.bCanEverTick = true;

    _State = EWallState::NoPressed;
}

int UWallTrapState::SwitchState_Implementation() {
    if(_State == EWallState::NoPressed) _State = EWallState::Pressed;
    else _State = EWallState::NoPressed;
    return 0;
}