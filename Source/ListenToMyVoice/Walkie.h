// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItfUsableItem.h"

#include "Components/ActorComponent.h"
#include "Walkie.generated.h"

DECLARE_EVENT(UWalkie, FRadioEvent);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UWalkie : public UActorComponent, public IItfUsableItem {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    UFMODEvent* _NoiseEvent;
    bool _AreDelegatesBinded;

    FDelegateHandle AddOnRadioDelegate(FRadioDelegate& RadioDelegate, bool IsPressed);
    void ClearOnRadioDelegate(FDelegateHandle DelegateHandle, bool IsPressed);

    UWalkie();

    /* Interfaces */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
    void UseItemPressed();
    virtual void UseItemPressed_Implementation() override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
    void UseItemReleased();
    virtual void UseItemReleased_Implementation() override;

protected:
    virtual void BeginPlay() override;

private:
    FRadioEvent _RadioPressedEvent;
    FRadioEvent _RadioReleasedEvent;
};
