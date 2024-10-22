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


// Removes from the last
void UMenuPanel::RemoveFrom(int From) {
    if (From >= 0 && From < _MenuInputs.Num()) {
        int Tot = _MenuInputs.Num();
        FVector Origin;
        FVector BoxExtent;
        float SphereRadius;
        UInputMenu* Pivot;
        for (int32 i = From; i < Tot; i++) {
            Pivot = _MenuInputs[i];
            UKismetSystemLibrary::GetComponentBounds(Pivot, Origin, BoxExtent, SphereRadius);
            _PanelHeight -= (2 * BoxExtent.Z) + _Margin;

            Pivot->ClearOnInputMenuDelegate();
            Pivot->DestroyComponent(true);
        }
        _MenuInputs.RemoveAt(From, _MenuInputs.Num() - From);
    }
}

int UMenuPanel::GetIndexOf(UInputMenu* Input) const {
    bool found = false;
    int i = 0;
    while (!found && i < _MenuInputs.Num()) {
        if (_MenuInputs[i] == Input) {
            found = true;
        }
        else i++;
    }
    return found ? i : -1;
}