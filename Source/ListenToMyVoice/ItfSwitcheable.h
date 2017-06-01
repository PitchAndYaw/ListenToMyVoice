#pragma once

#include "ItfSwitcheable.generated.h"


UINTERFACE(Blueprintable)
class LISTENTOMYVOICE_API UItfSwitcheable : public UInterface {
    GENERATED_UINTERFACE_BODY()
};

class LISTENTOMYVOICE_API IItfSwitcheable {
    GENERATED_IINTERFACE_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Switch")
    int SwitchState();
};


USTRUCT(BlueprintType, Category = "Switch")
struct FSwitcheable {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch")
    AActor* _actor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch")
    TArray<FName> _components;

    FSwitcheable() {
        _actor = nullptr;
        _components = {};
    }
};

USTRUCT(BlueprintType, Category = "Switch")
struct FSwitcher {
    GENERATED_USTRUCT_BODY()

private:
    TArray<UActorComponent*> SwitcheableComps;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch")
    TArray<FSwitcheable> SwitcheableArr;

    FSwitcher() {
        SwitcheableArr = {};
        SwitcheableComps = {};
    }

    void InitSwitcher() {
        /* Fill the references to other switcheable componets */
        TArray<UActorComponent*> components = {};
        for (FSwitcheable s : SwitcheableArr) {
            if (s._actor) s._actor->GetComponents(components);
            else ULibraryUtils::Log(TEXT("No switcheable actor"), 2);
            for (UActorComponent* comp : components) {
                if (s._components.IndexOfByKey(comp->GetFName()) != INDEX_NONE) {
                    SwitcheableComps.AddUnique(comp);
                }
            }
        }
    }

    void ActivateSwitcher() {
        for (UActorComponent* component : SwitcheableComps) {
            if (component->GetClass()->ImplementsInterface(UItfSwitcheable::StaticClass())) {
                IItfSwitcheable* itfObject = Cast<IItfSwitcheable>(component);
                if (itfObject) itfObject->Execute_SwitchState(component);
            }
        }
    }
};