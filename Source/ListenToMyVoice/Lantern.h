// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ItfUsableItem.h"

#include "Components/ActorComponent.h"
#include "Lantern.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LISTENTOMYVOICE_API ULantern : public UActorComponent, public IItfUsableItem {
	GENERATED_BODY()

public:
	ULantern();

    void PowerOff();

    void PowerOn();

    /*Inerfaces*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
    void UseItemPressed();
    virtual void UseItemPressed_Implementation() override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
    void UseItemReleased();
    virtual void UseItemReleased_Implementation() override;


protected:
    float _InnerLightIntensity;
    float _MiddleLightIntensity;
    float _OuterLightIntensity;

    USpotLightComponent* _InnerLight;
    USpotLightComponent* _MiddleLight;
    USpotLightComponent* _OuterLight;

	virtual void BeginPlay() override;

    bool _isLanternOn;
};
