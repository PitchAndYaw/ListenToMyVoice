// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"ItfSwitcheable.h"

#include "Components/ActorComponent.h"
#include "DoorState.generated.h"

UENUM()
namespace EOnAxis {
    enum Type {
    X_AXIS      UMETA(DisplayName = "X Axis"),
    Y_AXIS      UMETA(DisplayName = "Y Axis"),
    Z_AXIS      UMETA(DisplayName = "Z Axis")
    };
}

UENUM()
namespace EDoorType {
    enum Type {
        SLIDABLE_DOOR    UMETA(DisplayName = "Slidable"),
        ROTABLE_DOOR     UMETA(DisplayName = "Rotable")
    };
}

UENUM()
namespace EStateDoor {
	enum Type {
		CLOSE    UMETA(DisplayName = "Close"),
		OPENING     UMETA(DisplayName = "Opening"),
		OPEN     UMETA(DisplayName = "Open"),
		CLOSING     UMETA(DisplayName = "Closing")
	};
}

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UDoorState : public UActorComponent, public IItfSwitcheable {
    GENERATED_BODY()

private:
	UStaticMeshComponent* meshComp;
	FRotator Rotation;
	FVector Position;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation & Movement")
        TEnumAsByte<EDoorType::Type> DoorType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation & Movement")
        TEnumAsByte<EOnAxis::Type> ActOn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation & Movement")
		TEnumAsByte<EStateDoor::Type> StateDoor;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation & Movement")
        float _velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation & Movement")
		float _current_displacement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation & Movement")
		float _max_displacement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation & Movement")
		bool _block;


	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

    UDoorState();
    virtual void BeginPlay() override;

    /* Interfaces */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
    int SwitchState();
    virtual int SwitchState_Implementation() override;
};