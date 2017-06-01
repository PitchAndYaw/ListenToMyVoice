// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "LightState.h"


ULightState::ULightState() {
    PrimaryComponentTick.bCanEverTick = true;
    MAX_INTENSITY = 10.0f;
    _increment = MAX_INTENSITY;
    _requestOff = false;
}

void ULightState::BeginPlay() {
    Super::BeginPlay();

    _lightComp = Cast<ULightComponent>(GetOwner()->GetComponentByClass(ULightComponent::StaticClass()));
    ULibraryUtils::TestNull(_lightComp);
    _initialIntensity = _lightComp->Intensity;

    SetComponentTickEnabled(false);
}

void ULightState::TickComponent(float DeltaTime, ELevelTick TickType,
                                FActorComponentTickFunction* ThisTickFunction) {
    if (_lightComp->Intensity >= MAX_INTENSITY || _requestOff) {
        _lightComp->SetIntensity(_initialIntensity);
        SetComponentTickEnabled(false);
        _requestOff = false;
    }
    else {
        AddLightDelayed();
    }
}

int ULightState::SwitchState_Implementation() {
    if (_delayed) {
        if (_lightComp->Intensity > _initialIntensity &&
            _lightComp->Intensity < MAX_INTENSITY) {
            _requestOff = true;
        }
        else {
            SetComponentTickEnabled(true);
        }
    }
    else {
        AddLight();
    }

    return 0;
}

void ULightState::AddLight() {
    float i = _lightComp->Intensity + _increment;
    i = i > MAX_INTENSITY ? 0 : i;
    _lightComp->SetIntensity(i);
}

void ULightState::AddLightDelayed() {
    float i = _lightComp->Intensity + _increment;
    if (i >= MAX_INTENSITY) {
        i = MAX_INTENSITY;
        SetComponentTickEnabled(false);
    }
    _lightComp->SetIntensity(i);
}