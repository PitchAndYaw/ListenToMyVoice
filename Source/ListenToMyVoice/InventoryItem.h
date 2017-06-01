// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "InventoryItem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LISTENTOMYVOICE_API UInventoryItem : public UActorComponent{
	GENERATED_BODY()

public:	
    UPROPERTY(EditAnywhere)
    int _weight;

    UPROPERTY(EditAnywhere)
    bool _equipped;

    UPROPERTY(EditAnywhere)
    UTexture2D* _itemImage;

    UInventoryItem();

    virtual void BeginPlay() override;	

    void SetEquipped(bool equipped);
    
    bool IsEquipped();


    UFUNCTION(BlueprintCallable, Category = "InventoryItem")
    UTexture2D* GetItemImage();

private:
    
};
