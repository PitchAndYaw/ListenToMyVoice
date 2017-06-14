// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItfSwitcheable.h"

#include "Components/ActorComponent.h"
#include "DisplaceState.generated.h"


UENUM(BlueprintType)
enum class EDisplaceState : uint8 {
    InitialPosition UMETA(DisplayName = "Initial Position"),
    FinalPosition   UMETA(DisplayName = "Final Position"),
    Moving          UMETA(DisplayName = "Moving"),
    MovingReverse   UMETA(DisplayName = "Moving Reverse"),
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UDisplaceState : public UActorComponent, public IItfSwitcheable {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Displacement")
    FVector _DisplaceLocation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Displacement")
    FRotator _DisplaceRotation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Displacement")
    float _VelocityLocation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Displacement")
    float _VelocityRotation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Displacement")
    bool _Block;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Displacement")
    EDisplaceState _State;

    UDisplaceState();

    /* Interfaces */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Switcheable Interface")
    int SwitchState();
    virtual int SwitchState_Implementation() override;

protected:
    FVector _InitialLocation;
    FRotator _InitialRotation;
    FVector _FinalLocation;
    FRotator _FinalRotation;
    int _Direction;

    bool _LocationFinished;
    bool _RotationFinished;

    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
};
