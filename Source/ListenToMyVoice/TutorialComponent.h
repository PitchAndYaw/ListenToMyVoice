// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "TutorialComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UTutorialComponent : public UWidgetComponent {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steps")
    TArray<TSubclassOf<UUserWidget>> _WidgetSteps;

    UTutorialComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    void NextStep();

protected:
    int _StepPivot;

    void BeginPlay() override;

private:
    ACharacter* _Character;
};
