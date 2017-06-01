// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ItfSwitcheable.h"

#include "Components/ActorComponent.h"
#include "TextureState.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UTextureState : public UActorComponent, public IItfSwitcheable {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UTextureState();

    // Called when the game starts
    virtual void BeginPlay() override;

    /*INTERFACES*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Texture")
    int SwitchState();
    virtual int SwitchState_Implementation() override;
};
