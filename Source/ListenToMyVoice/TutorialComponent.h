// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "TutorialComponent.generated.h"


UENUM(BlueprintType)
enum class ETutorialCondition : uint8 {
    /* VR */
    Grab 	  UMETA(DisplayName = "VR Grab Item"),
    UseItemVR UMETA(DisplayName = "VR Use Item"),
    Drop	  UMETA(DisplayName = "VR Drop Item"),
    
    /* FP */
    Save      UMETA(DisplayName = "FP Save Item"),
    OpenInv   UMETA(DisplayName = "FP Open Inventory"),
    EquipItem UMETA(DisplayName = "FP Equip Item"),
    CloseInv  UMETA(DisplayName = "FP Close Inventory"),
    UseItemFP UMETA(DisplayName = "FP Use Item")
};

USTRUCT(BlueprintType, Category = "Tutorial")
struct FWidgetSteps {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETutorialCondition _Condition;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UUserWidget> _WidgetClass;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UTutorialComponent : public UWidgetComponent {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    AActor* _ActorRef;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    bool _IsVR;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Steps")
    TArray<FWidgetSteps> _WidgetSteps;

    UTutorialComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void NextStep();

protected:
    int _StepPivot;

    void BeginPlay() override;

private:
    UUserWidget* _Widget;
    ACharacter* _Character;

    bool CheckCondition();
};
