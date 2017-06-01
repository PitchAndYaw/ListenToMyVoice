// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class LISTENTOMYVOICE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
    //UInventoryWidget();
    void CreateWidget();

    //UFUNCTION(BlueprintCallable, Category = "Inventory")
    //Fstring GetFirstItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Widget")
    FString MyNewWidgetName;
	
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Widget")
    UHorizontalBox* SlotsContainer;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory Widget")
    UTexture2D* imagetexture;

    //UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
    //UInventory* GetInventory();
};
