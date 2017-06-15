// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItfSwitcheable.h"
#include "FMODAudioComponent.h"

#include "Components/ActorComponent.h"
#include "SoundState.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LISTENTOMYVOICE_API USoundState : public UFMODAudioComponent, public IItfSwitcheable {
    GENERATED_BODY()

public:
    USoundState();
    virtual void BeginPlay() override;
    
    /* Interfaces */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Switcheable Interface")
    int SwitchState();
    virtual int SwitchState_Implementation() override;

private:
    float _Parameter;
};
