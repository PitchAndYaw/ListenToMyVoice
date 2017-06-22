// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerControllerLobby.h"

#include "GameModeLobby.h"
#include "NWGameInstance.h"
#include "PlayerCharacter.h"
#include "FPCharacter.h"
#include "Menu3D.h"


APlayerControllerLobby::APlayerControllerLobby(const FObjectInitializer& OI) : Super(OI) {
    GConfig->GetString(
        TEXT("/Script/EngineSettings.GameMapsSettings"),
        TEXT("GameDefaultMap"),
        _MapMainMenu,
        GEngineIni
    );
}

void APlayerControllerLobby::SetupInputComponent() {
    Super::SetupInputComponent();
    InputComponent->BindAction("Menu", IE_Released, this, &APlayerControllerLobby::ToogleMenu);

    /* USE ITEM */
    InputComponent->BindAction("ClickLeft", IE_Pressed, this, &APlayerControllerLobby::UseLeftPressed);
    InputComponent->BindAction("ClickLeft", IE_Released, this, &APlayerControllerLobby::UseLeftReleased);
    InputComponent->BindAction("ClickRight", IE_Pressed, this, &APlayerControllerLobby::UseRightPressed);
    InputComponent->BindAction("ClickRight", IE_Released, this, &APlayerControllerLobby::UseRightReleased);
}

void APlayerControllerLobby::BeginPlay() {
    Super::BeginPlay();

    AFPCharacter* FPCharacter = Cast<AFPCharacter>(GetPawn());
    if (FPCharacter) {
        UUserWidget* HUD = CreateWidget<UUserWidget>(this, FPCharacter->_HUDClass);
        if (HUD) HUD->AddToViewport();
    }
}

void APlayerControllerLobby::CLIENT_InitialSetup_Implementation() {
    UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetGameInstance());
    if (GameInstance) SERVER_CallUpdate(GameInstance->_PlayerInfoSaved, false);
}

bool APlayerControllerLobby::SERVER_CallUpdate_Validate(FPlayerInfo info,
                                                        bool changedStatus) { return true; }
void APlayerControllerLobby::SERVER_CallUpdate_Implementation(FPlayerInfo info,
                                                              bool changedStatus) {
    AGameModeLobby* gameMode = Cast<AGameModeLobby>(GetWorld()->GetAuthGameMode());
    if (gameMode) gameMode->SERVER_SwapCharacter(this, info, changedStatus);
}

void APlayerControllerLobby::CLIENT_CreateMenu_Implementation() {
    /* MENU INTERFACE */
    if (GetPawnOrSpectator()) {
        CreateMenuActor(true);

        UCameraComponent* CameraComp = Cast<UCameraComponent>(GetPawnOrSpectator()->
                                                              FindComponentByClass<UCameraComponent>());
        if (CameraComp) {
            FVector Location = CameraComp->GetComponentLocation();
            Location.X += 300;
            UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetGameInstance());
            if (GameInstance && GameInstance->_IsVRMode) {
                Location.Z += 200;
                Location.X += 50;
            }
            _MenuActor->ToogleMenu(Location,
                                   CameraComp->GetComponentRotation(), false);

            APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
            if (PlayerCharacter) {
                PlayerCharacter->ToggleMenuInteraction(!_MenuActor->_IsMenuHidden);
                PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 10.0f;
            }
        }
    }
}

void APlayerControllerLobby::AfterPossessed() {
    /* CLIENT-SERVER EXCEPTION  */
    if (!_ClientPossesed) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            PlayerCharacter->AfterPossessed(false);
            _ClientPossesed = true;
        }
    }
}

void APlayerControllerLobby::OnRep_Pawn() {
    Super::OnRep_Pawn();
    /* CLIENT-SERVER EXCEPTION  */
    AfterPossessed();
}

/****************************************** ACTION MAPPINGS **************************************/
/******** USE ITEM LEFT *********/
void APlayerControllerLobby::UseLeftPressed() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool  IsMenuHidden = _MapMainMenu.Contains(GetWorld()->GetMapName()) ? false : 
                                _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseLeftPressed(IsMenuHidden);
    }
}

void APlayerControllerLobby::UseLeftReleased() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool  IsMenuHidden = _MapMainMenu.Contains(GetWorld()->GetMapName()) ? false :
                                _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseLeftReleased(IsMenuHidden);
        _IsLefReleased = true;
    }
}

/******** USE ITEM RIGHT *********/
void APlayerControllerLobby::UseRightPressed() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool  IsMenuHidden = _MapMainMenu.Contains(GetWorld()->GetMapName()) ? false :
            _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseRightPressed(IsMenuHidden);
    }
}

void APlayerControllerLobby::UseRightReleased() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool  IsMenuHidden = _MapMainMenu.Contains(GetWorld()->GetMapName()) ? false :
            _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseRightReleased(IsMenuHidden);
        _IsRightReleased = true;
    }
}

/*************** TRIGGER MENU *************/
void APlayerControllerLobby::ToogleMenu() {
    if (!_MapMainMenu.Contains(GetWorld()->GetMapName())) {
        /* MENU INTERFACE */
        if (!_MenuActor) CreateMenuActor(false);

        if (GetPawnOrSpectator()) {
            UCameraComponent* CameraComp = Cast<UCameraComponent>(GetPawnOrSpectator()->
                                                                  FindComponentByClass<UCameraComponent>());
            if (CameraComp) {
                FVector Location = CameraComp->GetComponentLocation() +
                                  (CameraComp->GetForwardVector().GetSafeNormal() * 200);
                
                _MenuActor->ToogleMenu(Location,
                                       CameraComp->GetComponentRotation());

                APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
                if (PlayerCharacter) PlayerCharacter->ToggleMenuInteraction(!_MenuActor->_IsMenuHidden);
            }
        }
    }
}

void APlayerControllerLobby::CreateMenuActor(bool IsMainMenu) {
    UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetGameInstance());
    if (GameInstance) {
        if (IsMainMenu) {
            _MenuActor = GameInstance->CreateMenuMain();
        }
        else {
            _MenuActor = GameInstance->CreateMenuLobby();
        }
    }
}