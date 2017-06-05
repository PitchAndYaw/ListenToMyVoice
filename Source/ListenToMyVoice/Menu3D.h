// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Menu3D.generated.h"


class UMenuPanel;
class UInputMenu;

UCLASS()
class LISTENTOMYVOICE_API AMenu3D : public AActor {
	GENERATED_BODY()
	
public:
    bool _IsMenuHidden;

    AMenu3D(const class FObjectInitializer& OI);
    void ToogleMenu(FVector Location, FRotator Rotation);

    void AddSubmenu(UMenuPanel* Submenu);
    void SetSubmenuByIndex(const int& Index);
    void SetInputMenuLoading(int IndexPanel, int IndexInputMenu, bool IsLoading, FString Text);

    int GetSubmenuNum();

protected:
    /*** DECORATORS ***/
    UPROPERTY(Category = "Menu Decorator", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* _TopDecorator;
    UPROPERTY(Category = "Menu Decorator", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* _BottomDecorator;
    UPROPERTY(Category = "Menu Decorator", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* _MiddleDecorator;

    UPROPERTY(Category = "Menu Decorator", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UInputMenu* _BackSubmenu;

    virtual void BeginPlay() override;

    /*** PANELS  ***/
    TArray<UMenuPanel*> _Submenus;

    /* BINDINGS */
    void OnButtonBack(UInputMenu* InputMenu);

private:
    float _BackMenuSize;
    TArray<int> _Breadcrumb;

    void PlaceDecorators(bool PlaceBackButton, float PanelHeight);
};
