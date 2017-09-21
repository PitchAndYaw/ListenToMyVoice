// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NWGameInstance.h"

#include "GameFramework/PlayerController.h"
#include "PlayerControllerLobby.generated.h"


UCLASS()
class LISTENTOMYVOICE_API APlayerControllerLobby : public APlayerController {
    GENERATED_BODY()

public:
    /* TUTORIAL */
    bool _IsLefReleased;
    bool _IsRightReleased;

    APlayerControllerLobby(const FObjectInitializer& OI);

    UFUNCTION()
    void OnRep_Pawn() override;

    UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Init")
    void CLIENT_Init();
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_InitCharacter(bool IsVR);

    UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Menu")
    void CLIENT_CreateMenu();

protected:

    virtual void SetupInputComponent() override;

    /********************************** ACTION MAPPINGS ******************************************/
    /******** USE ITEM LEFT *********/
    void UseLeftPressed();
    void UseLeftReleased();
    /******** USE ITEM RIGHT *********/
    void UseRightPressed();
    void UseRightReleased();

    /*************** TRIGGER MENU *************/
    void ToogleMenu();

private:
    bool _ClientPossesed;
    FString _MapMainMenu;

    /* MENU INTERFACE */
    class AMenu3D* _MenuActor;
    void CreateMenuActor(bool IsMainMenu);
};
