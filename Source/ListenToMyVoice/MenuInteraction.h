// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "MenuInteraction.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LISTENTOMYVOICE_API UMenuInteraction : public USceneComponent {
	GENERATED_BODY()

public:	
    float _RayParameter;

	UMenuInteraction();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Menu Interaction")
    void PressPointer();
    UFUNCTION(BlueprintCallable, Category = "Menu Interaction")
    void ReleasePointer();

protected:
    UPROPERTY(Category = "Light", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USpotLightComponent* _Light;

	virtual void BeginPlay() override;

private:
    bool _TargetLocked;
    class UInputMenu* _TargetInputMenu;

public:
    FORCEINLINE USpotLightComponent* UMenuInteraction::GetLight() const { return _Light; }
};
