// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "SwitcherComponent.h"


USwitcherComponent::USwitcherComponent() {
    PrimaryComponentTick.bCanEverTick = true;
}

void USwitcherComponent::BeginPlay() {
    Super::BeginPlay();
    _Switcher.InitSwitcher();
}

void USwitcherComponent::ActivateSwitcher() {
    _Switcher.ActivateSwitcher();
}