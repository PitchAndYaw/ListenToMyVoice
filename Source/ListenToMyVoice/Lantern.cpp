// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "Lantern.h"


ULantern::ULantern(){
    _batteryLife = 100.0;
    _isLanternOn = false;

    PrimaryComponentTick.bCanEverTick = true;
}


void ULantern::BeginPlay(){
	Super::BeginPlay();
    SetComponentTickEnabled(false);
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
    if (!_isLanternOn && _batteryLife > 0) {
        _isLanternOn = true;
        PowerOn();
        SetComponentTickEnabled(true);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Lantern On...")));
    }
    else if(_isLanternOn) {
        _isLanternOn = false;
        PowerOff();
        SetComponentTickEnabled(false);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Lantern Off...")));  
    }

}

void ULantern::PowerOff() {
    TArray<UActorComponent*> SpotLightArray;
    SpotLightArray = GetOwner()->GetComponentsByClass(USpotLightComponent::StaticClass());

    Cast<USpotLightComponent>(SpotLightArray[0])->SetIntensity(0);
    Cast<USpotLightComponent>(SpotLightArray[1])->SetIntensity(0);
    Cast<USpotLightComponent>(SpotLightArray[2])->SetIntensity(0);
}

void ULantern::PowerOn() {
    TArray<UActorComponent*> SpotLightArray;
    SpotLightArray = GetOwner()->GetComponentsByClass(USpotLightComponent::StaticClass());

    Cast<USpotLightComponent>(SpotLightArray[0])->SetIntensity(2333.0);
    Cast<USpotLightComponent>(SpotLightArray[1])->SetIntensity(8318.0);
    Cast<USpotLightComponent>(SpotLightArray[2])->SetIntensity(4527.0);
}