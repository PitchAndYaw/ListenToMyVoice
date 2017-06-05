// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/TextRenderComponent.h"
#include "TutorialText.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UTutorialText : public UTextRenderComponent {
    GENERATED_BODY()

public:
    UTutorialText();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

protected:
    void BeginPlay() override;

private:
    ACharacter* _Character;
};
