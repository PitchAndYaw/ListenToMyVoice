// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "UseSwitcher.h"

UUseSwitcher::UUseSwitcher() : Super() {
    PrimaryComponentTick.bCanEverTick = true;
}

void UUseSwitcher::BeginPlay() {
    Super::BeginPlay();
    _SwitcherPressed.InitSwitcher();
    _SwitcherReleased.InitSwitcher();
}

void UUseSwitcher::UsePressed_Implementation() {
    _SwitcherPressed.ActivateSwitcher();
}


void UUseSwitcher::UseReleased_Implementation() {
    _SwitcherReleased.ActivateSwitcher();
}
