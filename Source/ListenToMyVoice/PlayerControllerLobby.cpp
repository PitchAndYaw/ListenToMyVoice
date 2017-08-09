// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerControllerLobby.h"

#include "GameModeLobby.h"
#include "NWGameInstance.h"
#include "PlayerCharacter.h"
#include "Menu3D.h"


void APlayerControllerLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APlayerControllerLobby, _IsVR);
}

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

    UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetGameInstance());
    if (GameInstance) _IsVR = GameInstance->_IsVRMode;
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