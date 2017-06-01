// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItfSwitcheable.h"

#include "Components/ActorComponent.h"
#include "SwitcherComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API USwitcherComponent : public UActorComponent {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch")
    FSwitcher _Switcher;

    USwitcherComponent();

    UFUNCTION(BlueprintCallable, Category = "Switch")
    void ActivateSwitcher();

protected:
    virtual void BeginPlay() override;
};
