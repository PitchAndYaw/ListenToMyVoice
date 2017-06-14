// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItfSwitcheable.h"

#include "Components/ActorComponent.h"
#include "LightState.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LISTENTOMYVOICE_API ULightState : public UActorComponent, public IItfSwitcheable {
	GENERATED_BODY()

private:
    ULightComponent* _lightComp;
    float _initialIntensity;
    bool _requestOff;

    void AddLight();
    void AddLightDelayed();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Light)
    float MAX_INTENSITY;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Light)
    float _increment;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Light)
    bool _delayed;

	ULightState();
	virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    /* Interfaces */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Switcheable Interface")
    int SwitchState();
    virtual int SwitchState_Implementation() override;
};
