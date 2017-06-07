// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "TutorialComponent.generated.h"


UENUM(BlueprintType)
enum class ETutorialCondition : uint8 {
    /* COMMON */
    TakeItem UMETA(DisplayName = "Take Item"),
    UseItem  UMETA(DisplayName = "Use Item"),
    Use      UMETA(DisplayName = "Use"),

    /* VR */
    Drop	  UMETA(DisplayName = "VR Drop Item"),
    
    /* FP */
    Save      UMETA(DisplayName = "FP Save Item"),
    OpenInv   UMETA(DisplayName = "FP Open Inventory"),
    CloseInv  UMETA(DisplayName = "FP Close Inventory")
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    bool _IsMovable;
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
    class APlayerCharacter* _Character;

    bool CheckCondition();
};
