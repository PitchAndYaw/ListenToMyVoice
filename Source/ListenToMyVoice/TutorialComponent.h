// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "TutorialComponent.generated.h"


UENUM(BlueprintType)
enum class ETutorialCondition : uint8 {
    /* COMMON */
    ItemInHand      UMETA(DisplayName = "Equip Item"),
    NoItemInHand    UMETA(DisplayName = "UnEquip Item"),
    UseItemPressed  UMETA(DisplayName = "Use Item Pressed"),
    UseItemReleased UMETA(DisplayName = "Use Item Released"),
    UsePressed      UMETA(DisplayName = "Use Pressed"),
    UseReleased     UMETA(DisplayName = "Use Released"),

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    bool _IsDynamic;
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

    void CheckPlayer();
    bool CheckCondition();
};
