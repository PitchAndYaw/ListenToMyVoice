// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "Lantern.h"

#include "FMODAudioComponent.h"

ULantern::ULantern(){
    _batteryLife = 100.0;
    _isLanternOn = false;

    PrimaryComponentTick.bCanEverTick = true;

    _InnerLightIntensity = 0;
    _MiddleLightIntensity = 0;
    _OuterLightIntensity = 0;
}


void ULantern::BeginPlay(){
	Super::BeginPlay();
    SetComponentTickEnabled(false);

    TArray<UActorComponent*> SpotLightArray = GetOwner()->GetComponentsByClass(USpotLightComponent::StaticClass());
    USpotLightComponent* LightComponent;
    for (UActorComponent* Component : SpotLightArray) {
        LightComponent = Cast<USpotLightComponent>(Component);
        if (LightComponent->ComponentHasTag("inner")) {
            _InnerLight = LightComponent;
            _InnerLightIntensity = LightComponent->Intensity;
        }
        else if (LightComponent->ComponentHasTag("middle")) {
            _MiddleLight = LightComponent;
            _MiddleLightIntensity = LightComponent->Intensity;
        }
        else if (LightComponent->ComponentHasTag("outer")) {
            _OuterLight = LightComponent;
            _OuterLightIntensity = LightComponent->Intensity;
        }
    }
}


void ULantern::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    _batteryLife -= 0.01;
    UsingBattery();
}


/***************************************ACTIONS****************************************************/
void ULantern::UsingBattery() {    
    if (_batteryLife > 0.0 && _batteryLife < 1.0 || _batteryLife < 0) {
        SetComponentTickEnabled(false);

        _batteryLife = 0.0;  
        PowerOff();
        _isLanternOn = false;
    }
}

void ULantern::AddBatteryLife(float BatteryAmount) {
    _batteryLife += BatteryAmount;
}

float ULantern::GetBatteryLife() {
    return _batteryLife;
}


/******************Interfaces*****************/

void ULantern::UseItemPressed_Implementation() {}

void ULantern::UseItemReleased_Implementation() {
    UFMODAudioComponent* AudioComp = Cast<UFMODAudioComponent>(GetOwner()->GetComponentByClass(
        UFMODAudioComponent::StaticClass()));
    if (AudioComp) AudioComp->Play();

    if (!_isLanternOn && _batteryLife > 0) {
        _isLanternOn = true;
        PowerOn();
        SetComponentTickEnabled(true);
    }
    else if(_isLanternOn) {
        _isLanternOn = false;
        PowerOff();
        SetComponentTickEnabled(false);
    }

}

void ULantern::PowerOff() {
    if (_InnerLight) _InnerLight->SetIntensity(0);
    if (_MiddleLight) _MiddleLight->SetIntensity(0);
    if (_OuterLight) _OuterLight->SetIntensity(0);
}

void ULantern::PowerOn() {
    if (_InnerLight) _InnerLight->SetIntensity(_InnerLightIntensity);
    if (_MiddleLight) _MiddleLight->SetIntensity(_MiddleLightIntensity);
    if (_OuterLight) _OuterLight->SetIntensity(_OuterLightIntensity);
}