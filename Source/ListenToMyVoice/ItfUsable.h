#pragma once

#include "ItfUsable.generated.h"

UINTERFACE(Blueprintable)
class LISTENTOMYVOICE_API UItfUsable : public UInterface {
    GENERATED_UINTERFACE_BODY()
};

class LISTENTOMYVOICE_API IItfUsable {
    GENERATED_IINTERFACE_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use")
    void UsePressed();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use")
	void UseReleased();
};