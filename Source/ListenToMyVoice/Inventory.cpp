// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "Inventory.h"

#include "InventoryItem.h"

UInventory::UInventory() : Super() {
    _items = {};
    bReplicates = true;
    SetIsReplicated(true);
}

void UInventory::BeginPlay() {
    Super::BeginPlay();
}

void UInventory::AddItem(AActor* item) {
    ULibraryUtils::SetActorEnable(item, false);

    UInventoryItem* ItemToAdd = nullptr;

    ItemToAdd = Cast<UInventoryItem>(item->GetComponentByClass(
        UInventoryItem::StaticClass()));

    UStaticMeshComponent* itemMesh = Cast<UStaticMeshComponent>(item->GetComponentByClass(
        UStaticMeshComponent::StaticClass()));

    itemMesh->AttachToComponent(this,
        FAttachmentTransformRules::KeepRelativeTransform);

    itemMesh->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
    itemMesh->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);

    if (ItemToAdd && ItemToAdd->IsEquipped() == false) {
        _items.Add(item);
        ItemToAdd->SetEquipped(true);
    }
}

void UInventory::RemoveItem(AActor* itemToRemove) {

    for (AActor* item : _items) {
        if (itemToRemove == item) itemToRemove = item;
    }
    _items.Remove(itemToRemove);
}

UTexture2D* UInventory::GetItemTextureAt(int itemIndex) {
    UInventoryItem* comp;

    if (itemIndex < _items.Num()) {
       comp = Cast<UInventoryItem>(_items[itemIndex]->GetComponentByClass(
            UInventoryItem::StaticClass()));
       return comp->GetItemImage();
    }
    else return nullptr;
}

TArray<AActor*> UInventory::GetItemsArray() {
    return _items;
}

void UInventory::ShowAllItems() {
    for (AActor* item:_items){
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("ITEM: %s"), *item->GetName()));
        UE_LOG(LogTemp, Warning, TEXT("ITEM: %s"), *item->GetName());
    }
}

AActor* UInventory::PickItem(AActor* ItemActor) {
    AActor *ReturnItem = nullptr;
    UInventoryItem* ItemComp = nullptr;
    
    for (AActor* item : _items) {
        ItemComp = Cast<UInventoryItem>(item->FindComponentByClass(UInventoryItem::StaticClass()));

        if (item == ItemActor) {
            ReturnItem = item;
            return ReturnItem;
        }
    }

    return ReturnItem;
}

UActorComponent* UInventory::GetInventoryItemComponent(AActor* item) {

    if(item->GetComponentByClass(UInventoryItem::StaticClass()))
        return Cast<UActorComponent>(item->GetComponentByClass(UInventoryItem::StaticClass()));

    else return nullptr;
}