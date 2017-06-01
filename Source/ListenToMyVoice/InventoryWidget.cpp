// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "InventoryWidget.h"
#include "Inventory.h"

void UInventoryWidget::CreateWidget() {

    UImage* image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    image->SetColorAndOpacity(FLinearColor(0, 0, 0, 0));

    UPanelWidget* root = Cast<UPanelWidget>(GetRootWidget());
    UPanelSlot* slot = root->AddChild(image);
    UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(slot);

    // set position and stuff
    canvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
    canvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    canvasSlot->SetOffsets(FMargin(0, 0, 200, 200));

}

