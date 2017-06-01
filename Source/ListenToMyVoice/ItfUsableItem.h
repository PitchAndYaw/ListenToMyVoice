#pragma once

#include "ItfUsableItem.generated.h"

UINTERFACE(Blueprintable)
class LISTENTOMYVOICE_API UItfUsableItem : public UInterface {
    GENERATED_UINTERFACE_BODY()
};

class LISTENTOMYVOICE_API IItfUsableItem {
    GENERATED_IINTERFACE_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
    void UseItemPressed();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
    void UseItemReleased();
};