// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItfSwitcheable.h"

#include "Components/ActorComponent.h"
#include "WallTrapState.generated.h"

UENUM(BlueprintType)
enum class EWallState : uint8 {
    NoPressed,
    Pressed
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UWallTrapState : public UActorComponent, public IItfSwitcheable {
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EWallState _State;

    UWallTrapState();

    /* Interfaces */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    int SwitchState();
    virtual int SwitchState_Implementation() override;
};
