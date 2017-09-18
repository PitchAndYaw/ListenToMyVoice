// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "MenuPanel.generated.h"


class UInputMenu;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UMenuPanel : public USceneComponent {
    GENERATED_BODY()

public:
    float _Margin;
    float _PanelHeight;

    UMenuPanel();
    void AddMenuInput(UInputMenu* NewSlot);

    void EnablePanel(bool Enable);
    UInputMenu* GetInputMenuAt(int Index);
    UInputMenu* GetInputMenuLast();
    int GetIndexOf(UInputMenu* Input) const;
    void RemoveFrom(int From);

protected:
    TArray<UInputMenu*> _MenuInputs;
};
