// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ItfSwitcheable.h"

#include "Components/ActorComponent.h"
#include "LightPressState.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API ULightPressState : public UActorComponent, public IItfSwitcheable {

		GENERATED_BODY()

private:
	ULightComponent* _lightComp;
	bool _on;

public:
	// Sets default values for this component's properties
	ULightPressState();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Light)
	bool _current_state;
	
    /* Interfaces */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	int SwitchState();
	virtual int SwitchState_Implementation() override;
};
