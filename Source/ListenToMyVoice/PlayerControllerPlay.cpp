#include "ListenToMyVoice.h"
#include "PlayerControllerPlay.h"

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
    InputComponent->BindAction("Menu", IE_Released, this, &APlayerControllerPlay::ToggleMenu);

    /* USE ITEM */
    InputComponent->BindAction("ClickLeft", IE_Pressed, this, &APlayerControllerPlay::UseLeftPressed);
    InputComponent->BindAction("ClickLeft", IE_Released, this, &APlayerControllerPlay::UseLeftReleased);
    InputComponent->BindAction("ClickRight", IE_Pressed, this, &APlayerControllerPlay::UseRightPressed);
    InputComponent->BindAction("ClickRight", IE_Released, this, &APlayerControllerPlay::UseRightReleased);
}

void APlayerControllerPlay::OnRep_Pawn() {
    Super::OnRep_Pawn();

    if (!_ClientPossesed) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            PlayerCharacter->_OnRadioPressedDelegate.BindUObject(this, &APlayerControllerPlay::OnRadioPressed);
            PlayerCharacter->_OnRadioReleasedDelegate.BindUObject(this, &APlayerControllerPlay::OnRadioReleased);
            PlayerCharacter->AfterPossessed(true);
            _ClientPossesed = true;
        }
    }
    /* PLAYER DEAD */
    else {
        APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetPawn());
        if (PlayerSpectator) {
            PlayerSpectator->AfterPossessed();
            /* MENU INTERFACE */
            bool  IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
            if (!IsMenuHidden) ToggleMenu();
            ToggleMenu();
        }
    }
}

bool APlayerControllerPlay::GetOtherPlayerState() {
    bool found = false;
    if (!_OtherPlayerState) {
        for (APlayerState* OtherPlayerState : GetWorld()->GetGameState()->PlayerArray) {
            if (PlayerState->UniqueId != OtherPlayerState->UniqueId &&
                PlayerState->IsA(APlayerStatePlay::StaticClass())) {
                
                _OtherPlayerState = Cast<APlayerStatePlay>(OtherPlayerState);
                found = true;
            }
        }
    }
    else found = true;
    return found;
}

/*********************************************** VOICE *******************************************/
void APlayerControllerPlay::ModifyVoiceAudioComponent(const FUniqueNetId& RemoteTalkerId,
                                                      class UAudioComponent* AudioComponent) {

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    bool FullVolume = false;
    if (PlayerCharacter) {
        FullVolume = PlayerCharacter->IsWalkieInHand();

        if (!_VoiceAudioComp && GetOtherPlayerState()) {
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
                    AudioComponent->AttenuationOverrides.OmniRadius = 15;
                    AudioComponent->AttenuationOverrides.FalloffDistance = 2000;
                    _VoiceAudioComp = AudioComponent;

                    _WalkieNoiseAudioComp->SetEvent(NoiseEvent);
                    _WalkieNoiseAudioComp->AttachToComponent(MeshComponent,
                                                             FAttachmentTransformRules::KeepRelativeTransform);
                    _WalkieNoiseAudioComp->bOverrideAttenuation = true;
                    ULibraryUtils::Log("Setup Voice");
                }
            }
            else if(AudioComponent) {
                AudioComponent->SetVolumeMultiplier(0);
                ULibraryUtils::Log("VOLUME: 0");
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
    if (IsLocalPlayerController()) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
            PlayerCharacter->UseLeftPressed(IsMenuHidden);
        }
        else {
            APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetPawn());
            if (PlayerSpectator) PlayerSpectator->ClickLeftPressed();
        }
    }
}

void APlayerControllerPlay::UseLeftReleased() {
    if (IsLocalPlayerController()) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
            PlayerCharacter->UseLeftReleased(IsMenuHidden);
        }
        else {
            APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetPawn());
            if (PlayerSpectator) PlayerSpectator->ClickLeftReleased();
        }
    }
}

/******* USE ITEM RIGHT *********/
void APlayerControllerPlay::UseRightPressed() {
    if (IsLocalPlayerController()) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
            PlayerCharacter->UseRightPressed(IsMenuHidden);
        }
        else {
            APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetPawn());
            if (PlayerSpectator) PlayerSpectator->ClickLeftPressed();
        }
    }
}

void APlayerControllerPlay::UseRightReleased() {
    if (IsLocalPlayerController()) {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
        if (PlayerCharacter) {
            bool IsMenuHidden = _MenuActor ? _MenuActor->_IsMenuHidden : true;
            PlayerCharacter->UseRightReleased(IsMenuHidden);
        }
        else {
            APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetPawn());
            if (PlayerSpectator) PlayerSpectator->ClickLeftReleased();
        }
    }
}

/*************** TRIGGER MENU *************/
void APlayerControllerPlay::ToggleMenu() {
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    if (PlayerCharacter && !PlayerCharacter->_ItemPressedLeft && !PlayerCharacter->_ItemPressedRight) {
        /* MENU INTERFACE */
        if (!_MenuActor) CreateMenuActor();

        UCameraComponent* CameraComp = Cast<UCameraComponent>(PlayerCharacter->
                                                              FindComponentByClass<UCameraComponent>());
        if (CameraComp) {
            FVector Location = CameraComp->GetComponentLocation() +
                (CameraComp->GetForwardVector().GetSafeNormal() * 200);

            _MenuActor->ToggleMenu(Location, CameraComp->GetComponentRotation());
            PlayerCharacter->ToggleMenuInteraction(!_MenuActor->_IsMenuHidden);
        }
    }
    else {
        APlayerSpectator* PlayerSpectator = Cast<APlayerSpectator>(GetPawn());
        if (PlayerSpectator) {
            /* MENU INTERFACE */
            if (!_MenuActor) CreateMenuActor();

            UCameraComponent* CameraComp = Cast<UCameraComponent>(PlayerSpectator->
                                                                  FindComponentByClass<UCameraComponent>());
            if (CameraComp) {
                FVector Location = CameraComp->GetComponentLocation() +
                    (CameraComp->GetForwardVector().GetSafeNormal() * 200);

                _MenuActor->ToggleMenu(Location, CameraComp->GetComponentRotation());
                PlayerSpectator->ToggleMenuInteraction(!_MenuActor->_IsMenuHidden);
            }
        }
    }
}

void APlayerControllerPlay::CreateMenuActor() {
    UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetGameInstance());
    if (GameInstance) _MenuActor = GameInstance->CreateMenuPlay();
}

/*********************************************** DELEGATES ***************************************/
void APlayerControllerPlay::OnRadioPressed() {
    APlayerStatePlay* PS = Cast<APlayerStatePlay>(PlayerState);
    if (PS) PS->SERVER_SetIsTalking(true);

    StartTalking();

    ULibraryUtils::Log(FString::Printf(TEXT("I AM: %s"),
                                       *PlayerState->UniqueId.ToDebugString()), 3, 60);

    if (GetOtherPlayerState()) {
        ClientMutePlayer(_OtherPlayerState->UniqueId);
        ULibraryUtils::Log(FString::Printf(TEXT("MUTE: %s"),
                                           *_OtherPlayerState->UniqueId.ToDebugString()), 2, 60);
    }
}

void APlayerControllerPlay::OnRadioReleased() {
    APlayerStatePlay* PS = Cast<APlayerStatePlay>(PlayerState);
    if (PS) PS->SERVER_SetIsTalking(false);

    StopTalking();

    if (GetOtherPlayerState()) {
        ClientUnmutePlayer(_OtherPlayerState->UniqueId);
        ULibraryUtils::Log(FString::Printf(TEXT("UNMUTE: %s"),
                                           *_OtherPlayerState->UniqueId.ToDebugString()), 0, 60);
    }
}