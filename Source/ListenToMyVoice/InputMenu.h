// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "InputMenu.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LISTENTOMYVOICE_API UInputMenu : public UStaticMeshComponent {
    GENERATED_BODY()

public:
    DECLARE_EVENT_OneParam(UInputMenu, FInputMenuEvent, UInputMenu*);
    DECLARE_DELEGATE_OneParam(FInputMenuDelegate, UInputMenu*);


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text Menu")
    UTextRenderComponent* _TextRender;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text Menu")
    FColor _Color;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text Menu")
    FColor _HoverColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Menu")
    class UFMODAudioComponent* _AudioComp;

    FInputMenuEvent _InputMenuPressedEvent;
    FInputMenuEvent _InputMenuReleasedEvent;

    FDelegateHandle _OnInputMenuPressedDelegateHandle;
    FDelegateHandle _OnInputMenuReleasedDelegateHandle;

    FInputMenuDelegate _InputMenuPressedDelegate;
    FInputMenuDelegate _InputMenuReleasedDelegate;

    UInputMenu(const FObjectInitializer& OI);

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    void Enable(bool Enable);
    void SetLoading(bool IsLoading, FString Text);

    void AddOnInputMenuDelegate();
    void ClearOnInputMenuDelegate();

    void HoverInteraction();
    void EndhoverInteraction();

    void ReleaseEvents();
    void PressEvents();

    void Init(const FVector MenuPanelLocation);

    //UFUNCTION()
    //void OnActivate(UActorComponent* Component, bool bReset);
    //UFUNCTION()
    //void OnDeactivate(UActorComponent* Component);

protected:
    void BeginPlay() override;

private:
    FVector _InitialLocation;
    FVector _NextPoint;
    bool _IsFlee;

    FText _PrevText;
    bool _IsLoading;

    void UpdateNextPoint();
};
