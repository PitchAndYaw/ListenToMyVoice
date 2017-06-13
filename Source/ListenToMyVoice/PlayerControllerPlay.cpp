// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerControllerPlay.h"

#include "GameModePlay.h"
#include "NWGameInstance.h"
#include "FMODAudioComponent.h"
#include "PlayerCharacter.h"
#include "PlayerSpectator.h"
#include "PlayerStatePlay.h"


APlayerControllerPlay::APlayerControllerPlay(const FObjectInitializer& OI) : Super(OI) {
    /* VOICE */
    _WalkieNoiseAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio"));
    _WalkieNoiseAudioComp->bAutoActivate = false;
    _ClientPossesed = false;
}

void APlayerControllerPlay::SetupInputComponent() {
    Super::SetupInputComponent();
    InputComponent->BindAction("Menu", IE_Released, this, &APlayerControllerPlay::ToogleMenu);

    /* USE ITEM */
    InputComponent->BindAction("ClickLeft", IE_Pressed, this, &APlayerControllerPlay::UseLeftPressed);
    InputComponent->BindAction("ClickLeft", IE_Released, this, &APlayerControllerPlay::UseLeftReleased);
    InputComponent->BindAction("ClickRight", IE_Pressed, this, &APlayerControllerPlay::UseRightPressed);
    InputComponent->BindAction("ClickRight", IE_Released, this, &APlayerControllerPlay::UseRightReleased);
}

void APlayerControllerPlay::BeginPlay() {
    Super::BeginPlay();

    if (IsLocalController()) {
        _GameInstance = Cast<UNWGameInstance>(GetGameInstance());
        if (_GameInstance) {
            SERVER_CallUpdate(_GameInstance->_PlayerInfoSaved);
        }
    }
}

bool APlayerControllerPlay::SERVER_CallUpdate_Validate(FPlayerInfo info) {
    return true;
}
void APlayerControllerPlay::SERVER_CallUpdate_Implementation(FPlayerInfo info) {
    AGameModePlay* gameMode = Cast<AGameModePlay>(GetWorld()->GetAuthGameMode());
    if (gameMode) gameMode->SERVER_RespawnPlayer(this, info);
}

void APlayerControllerPlay::AfterPossessed() {
    /* CLIENT-SERVER EXCEPTION */
    if (!_ClientPossesed) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (!_GameInstance || !PlayerCharacter) return;

        if (PlayerCharacter->IsA(_GameInstance->_PlayerInfoSaved.CharacterClass)) {
            PlayerCharacter->_OnRadioPressedDelegate.BindUObject(this, &APlayerControllerPlay::OnRadioPressed);
            PlayerCharacter->_OnRadioReleasedDelegate.BindUObject(this, &APlayerControllerPlay::OnRadioReleased);
            PlayerCharacter->AfterPossessed(true);
            _ClientPossesed = true;
        }
    }
}

void APlayerControllerPlay::OnRep_Pawn() {
    Super::OnRep_Pawn();
    /* CLIENT-SERVER EXCEPTION */
    AfterPossessed();
}

/*********************************************** VOICE *******************************************/
void APlayerControllerPlay::ModifyVoiceAudioComponent(const FUniqueNetId& RemoteTalkerId,
                                                      class UAudioComponent* AudioComponent) {

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    bool FullVolume = false;
    if (PlayerCharacter) {
        FullVolume = PlayerCharacter->IsWalkieInHand();

        if (!_VoiceAudioComp) {
            AActor* WalkieActor = PlayerCharacter->GetWalkieActor();
            if (WalkieActor) {
                UFMODEvent* NoiseEvent = PlayerCharacter->GetWalkieEvent();
                UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(
                    WalkieActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

                _WalkieLight = Cast<UPointLightComponent>(
                    WalkieActor->GetComponentByClass(UPointLightComponent::StaticClass()));
                _WalkieLight->SetIntensity(0);

                if (MeshComponent && NoiseEvent) {
                    AudioComponent->AttachToComponent(MeshComponent,
                                                      FAttachmentTransformRules::KeepRelativeTransform);
                    AudioComponent->bOverrideAttenuation = true;
                    _VoiceAudioComp = AudioComponent;

                    _WalkieNoiseAudioComp->SetEvent(NoiseEvent);
                    _WalkieNoiseAudioComp->AttachToComponent(MeshComponent,
                                                             FAttachmentTransformRules::KeepRelativeTransform);
                    _WalkieNoiseAudioComp->bOverrideAttenuation = true;
                    ULibraryUtils::Log("Setup Voice");
                }
            }
        }

        if (FullVolume) {
            if (_VoiceAudioComp) {
                _VoiceAudioComp->SetVolumeMultiplier(1.0);
                _WalkieNoiseAudioComp->SetVolume(1.0);
                ULibraryUtils::Log("VOLUME: 1.0");
            }
        }
        else {
            if (_VoiceAudioComp) {
                _VoiceAudioComp->SetVolumeMultiplier(0.05);
                _WalkieNoiseAudioComp->SetVolume(0.05);
                ULibraryUtils::Log("VOLUME: 0.05");
            }
        }
    }
}

void APlayerControllerPlay::TickActor(float DeltaTime, enum ELevelTick TickType,
                                      FActorTickFunction & ThisTickFunction) {
    Super::TickActor(DeltaTime, TickType, ThisTickFunction);
    TickWalkie();
}

void APlayerControllerPlay::TickWalkie() {
    /* Noise Event */
    if (_VoiceAudioComp && _WalkieNoiseAudioComp) {
        if (_VoiceAudioComp->IsPlaying() && !_WalkieNoiseAudioComp->IsPlaying()) {
            _WalkieNoiseAudioComp->Play();
        }
        else if (!_VoiceAudioComp->IsPlaying() && _WalkieNoiseAudioComp->IsPlaying()) {
            _WalkieNoiseAudioComp->Stop();
        }
    }

    /* Walkie Light */
    if (!_WalkieLight) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            AActor* WalkieActor = PlayerCharacter->GetWalkieActor();
            if (WalkieActor) {
                _WalkieLight = Cast<UPointLightComponent>(
                    WalkieActor->GetComponentByClass(UPointLightComponent::StaticClass()));
                _WalkieLight->SetIntensity(0);
            }
        }
    }

    if (_WalkieLight) SetLightColor();
}

void APlayerControllerPlay::SetLightColor() {
    APlayerStatePlay* PS = Cast<APlayerStatePlay>(PlayerState);
    if (PS && _OtherPlayerState) {
        if (PS->GetIsTalking()) {
            if (_OtherPlayerState->GetIsTalking()) {
                _WalkieLight->SetIntensity(5000);
                _WalkieLight->SetLightColor(FLinearColor::Red);
            }
            else {
                _WalkieLight->SetIntensity(5000);
                _WalkieLight->SetLightColor(FLinearColor::Green);
            }
        }
        else {
            if (_OtherPlayerState->GetIsTalking()) {
                _WalkieLight->SetIntensity(5000);
                _WalkieLight->SetLightColor(FLinearColor::Blue);
            }
            else {
                _WalkieLight->SetIntensity(0);
            }
        }
    }
}

/****************************************** ACTION MAPPINGS **************************************/

/******** USE ITEM LEFT *********/
void APlayerControllerPlay::UseLeftPressed() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseLeftPressed(IsMenuHidden);
    }
    else {
        APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetSpectatorPawn());
        if (PlayerSpectator) PlayerSpectator->ClickLeftPressed();
    }
}

void APlayerControllerPlay::UseLeftReleased() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseLeftReleased(IsMenuHidden);
    }
    else {
        APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetSpectatorPawn());
        if (PlayerSpectator) PlayerSpectator->ClickLeftReleased();
    }
}

/******* USE ITEM RIGHT *********/
void APlayerControllerPlay::UseRightPressed() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseRightPressed(IsMenuHidden);
    }
    else {
        APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetSpectatorPawn());
        if (PlayerSpectator) PlayerSpectator->ClickLeftPressed();
    }
}

void APlayerControllerPlay::UseRightReleased() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
        PlayerCharacter->UseRightReleased(IsMenuHidden);
    }
    else {
        APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetSpectatorPawn());
        if (PlayerSpectator) PlayerSpectator->ClickLeftReleased();
    }
}

/*************** TRIGGER MENU *************/
void APlayerControllerPlay::ToogleMenu() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter) {
        /* MENU INTERFACE */
        if(!_MenuActor) CreateMenuActor();

        UCameraComponent* CameraComp = Cast<UCameraComponent>(PlayerCharacter->
                                                              FindComponentByClass<UCameraComponent>());
        if (CameraComp) {
            FVector Location = CameraComp->GetComponentLocation() +
                              (CameraComp->GetForwardVector().GetSafeNormal() * 200);

            _MenuActor->ToogleMenu(Location,
                                   CameraComp->GetComponentRotation());
            PlayerCharacter->ToggleMenuInteraction(!_MenuActor->_IsMenuHidden);
        }
    }
}

void APlayerControllerPlay::CreateMenuActor() {
    UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetGameInstance());
    if (GameInstance) _MenuActor = GameInstance->CreateMenuPlay();
}

void APlayerControllerPlay::CLIENT_ShowMenu_Implementation() {
    if (!_MenuActor) CreateMenuActor();
    if (_MenuActor->_IsMenuHidden) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            UCameraComponent* CameraComp = Cast<UCameraComponent>(PlayerCharacter->
                                                                  FindComponentByClass<UCameraComponent>());
            if (CameraComp) {
                FVector Location = CameraComp->GetComponentLocation() +
                                  (CameraComp->GetForwardVector().GetSafeNormal() * 200);

                _MenuActor->ToogleMenu(Location,
                                       CameraComp->GetComponentRotation());
                PlayerCharacter->ToggleMenuInteraction(true);
            }
        }
    }
}

void APlayerControllerPlay::CLIENT_HideMenu_Implementation() {
    bool  IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
    if (!IsMenuHidden) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            UCameraComponent* CameraComp = Cast<UCameraComponent>(PlayerCharacter->
                                                                  FindComponentByClass<UCameraComponent>());
            if (CameraComp) {
                _MenuActor->ToogleMenu(CameraComp->GetComponentLocation(),
                                       CameraComp->GetComponentRotation());
                PlayerCharacter->ToggleMenuInteraction(false);
            }
        }
    }
}

/*********************************************** DELEGATES ***************************************/
void APlayerControllerPlay::OnRadioPressed() {
    APlayerStatePlay* PS = Cast<APlayerStatePlay>(PlayerState);
    if (PS) PS->SetIsTalking(true);

    StartTalking();

    ULibraryUtils::Log(FString::Printf(TEXT("I AM: %s"),
                                       *PlayerState->UniqueId.ToDebugString()), 3, 60);

    if (!_OtherPlayerState) {
        for (APlayerState* OtherPlayerState : GetWorld()->GetGameState()->PlayerArray) {
            if (PlayerState->UniqueId != OtherPlayerState->UniqueId && PlayerState->IsA(APlayerStatePlay::StaticClass())) {
                _OtherPlayerState = Cast<APlayerStatePlay>(OtherPlayerState);
            }
        }
    }

    if (_OtherPlayerState) {
        ClientMutePlayer(_OtherPlayerState->UniqueId);
        ULibraryUtils::Log(FString::Printf(TEXT("MUTE: %s"),
                                           *_OtherPlayerState->UniqueId.ToDebugString()), 2, 60);
    }
}

void APlayerControllerPlay::OnRadioReleased() {
    APlayerStatePlay* PS = Cast<APlayerStatePlay>(PlayerState);
    if (PS) PS->SetIsTalking(false);

    StopTalking();

    if (!_OtherPlayerState) {
        for (APlayerState* OtherPlayerState : GetWorld()->GetGameState()->PlayerArray) {
            if (PlayerState->UniqueId != OtherPlayerState->UniqueId && PlayerState->IsA(APlayerStatePlay::StaticClass())) {
                _OtherPlayerState = Cast<APlayerStatePlay>(OtherPlayerState);
            }
        }
    }

    if (_OtherPlayerState) {
        ClientUnmutePlayer(_OtherPlayerState->UniqueId);
        ULibraryUtils::Log(FString::Printf(TEXT("UNMUTE: %s"),
                                           *_OtherPlayerState->UniqueId.ToDebugString()), 0, 60);
    }
}

/******************************************** GAME FLOW ******************************************/
void APlayerControllerPlay::CLIENT_Dead_Implementation() {
    bool  IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
    APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetSpectatorPawn());
    if (IsMenuHidden && PlayerSpectator) {
        /* MENU INTERFACE */
        if (!_MenuActor) CreateMenuActor();

        UCameraComponent* CameraComp = Cast<UCameraComponent>(PlayerSpectator->
                                                              FindComponentByClass<UCameraComponent>());
        if (CameraComp) {
            FVector Location = CameraComp->GetComponentLocation() +
                (CameraComp->GetForwardVector().GetSafeNormal() * 200);

            _MenuActor->ToogleMenu(Location,
                                   CameraComp->GetComponentRotation());
            PlayerSpectator->ToggleMenuInteraction(true);
        }
    }
}

void APlayerControllerPlay::CLIENT_GotoState_Implementation(FName NewState) {
    if (GetPawn()) {// CLIENT-SERVER EXCEPTION
        FVector Location = GetPawn()->GetActorLocation();
        ClientGotoState(NewState);
        GetSpectatorPawn()->SetActorLocation(Location);
        CLIENT_Dead();
    }
}