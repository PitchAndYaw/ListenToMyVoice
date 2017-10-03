// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerControllerLobby.h"

#include "NWGameInstance.h"
#include "PlayerCharacter.h"
#include "GameModeLobby.h"


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
    InputComponent->BindAction("Menu", IE_Released, this, &APlayerControllerLobby::ToggleMenu);

    /* USE ITEM */
    InputComponent->BindAction("ClickLeft", IE_Pressed, this, &APlayerControllerLobby::UseLeftPressed);
    InputComponent->BindAction("ClickLeft", IE_Released, this, &APlayerControllerLobby::UseLeftReleased);
    InputComponent->BindAction("ClickRight", IE_Pressed, this, &APlayerControllerLobby::UseRightPressed);
    InputComponent->BindAction("ClickRight", IE_Released, this, &APlayerControllerLobby::UseRightReleased);
}

void APlayerControllerLobby::CLIENT_Init_Implementation() {
    UNWGameInstance* GI = Cast<UNWGameInstance>(GetGameInstance());
    if (GI) {
        SERVER_InitCharacter(GI->_IsVRMode);
    }
}

bool APlayerControllerLobby::SERVER_InitCharacter_Validate(bool IsVR) { return true; }
void APlayerControllerLobby::SERVER_InitCharacter_Implementation(bool IsVR) {
    AGameModeLobby* GM = Cast<AGameModeLobby>(GetGameInstance()->GetWorld()->GetAuthGameMode());
    if (GM) {
        GM->SERVER_SpawnCharacter(IsVR, this);
    }
}

void APlayerControllerLobby::OnRep_Pawn() {
    Super::OnRep_Pawn();

    if (!_ClientPossesed) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            PlayerCharacter->AfterPossessed(false);
            _ClientPossesed = true;
        }
    }
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
            _MenuActor->ToggleMenu(Location,
                                   CameraComp->GetComponentRotation(), false);

            APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
            if (PlayerCharacter) {
                PlayerCharacter->ToggleMenuInteraction(!_MenuActor->_IsMenuHidden);
                PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 10.0f;
            }
        }
    }
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
void APlayerControllerLobby::ToggleMenu() {
    if (!_MapMainMenu.Contains(GetWorld()->GetMapName())) {
        /* MENU INTERFACE */
        if (!_MenuActor) CreateMenuActor(false);

        if (GetPawnOrSpectator()) {
            UCameraComponent* CameraComp = Cast<UCameraComponent>(GetPawnOrSpectator()->
                                                                  FindComponentByClass<UCameraComponent>());
            if (CameraComp) {
                FVector Location = CameraComp->GetComponentLocation() +
                                  (CameraComp->GetForwardVector().GetSafeNormal() * 200);
                
                _MenuActor->ToggleMenu(Location,
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