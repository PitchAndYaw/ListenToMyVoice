// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "MenuPanel.h"

#include "InputMenu.h"

UMenuPanel::UMenuPanel() : Super() {
    _MenuInputs = {};

    _Margin = 10;
    _PanelHeight = _Margin;
}

void UMenuPanel::AddMenuInput(UInputMenu* NewSlot) {
    NewSlot->RegisterComponent();

    NewSlot->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
    NewSlot->Init(FVector(0, 0, -_PanelHeight));

    _MenuInputs.Add(NewSlot);

    FVector Origin;
    FVector BoxExtent;
    float SphereRadius;

    UKismetSystemLibrary::GetComponentBounds(NewSlot, Origin, BoxExtent, SphereRadius);
    _PanelHeight += (2*BoxExtent.Z) +  _Margin;
}

void UMenuPanel::EnablePanel(bool Enable) {
    for (UInputMenu* InputMenu : _MenuInputs) {
        InputMenu->Enable(Enable);
    }
}

UInputMenu* UMenuPanel::GetInputMenuAt(int Index) {
    return Index < _MenuInputs.Num() ? _MenuInputs[Index] : nullptr;

}

UInputMenu* UMenuPanel::GetInputMenuLast() {
    return _MenuInputs.Top();
}