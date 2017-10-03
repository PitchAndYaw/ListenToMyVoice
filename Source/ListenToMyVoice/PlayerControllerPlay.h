// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "PlayerControllerPlay.generated.h"


UCLASS()
class LISTENTOMYVOICE_API APlayerControllerPlay : public APlayerController {
    GENERATED_BODY()

public:
    APlayerControllerPlay(const FObjectInitializer& OI);

    virtual void TickActor(float DeltaTime, enum ELevelTick TickType,
                           FActorTickFunction & ThisTickFunction) override;

    UFUNCTION()
    void OnRep_Pawn() override;

    /*************************************** VOICE ***********************************************/
    virtual void ModifyVoiceAudioComponent(const FUniqueNetId& RemoteTalkerId,
                                           class UAudioComponent* AudioComponent) override;

    /* Radio Delegate */
    void OnRadioPressed();
    void OnRadioReleased();

    /*************** TRIGGER MENU *************/
    void ToggleMenu();

protected:

    virtual void SetupInputComponent() override;

    /********************************** ACTION MAPPINGS ******************************************/
    /******** USE ITEM LEFT *********/
    void UseLeftPressed();
    void UseLeftReleased();

    /******* USE ITEM RIGHT *********/
    void UseRightPressed();
    void UseRightReleased();

private:
    UPointLightComponent* _WalkieLight;

    class UFMODAudioComponent* _WalkieNoiseAudioComp;
    UAudioComponent* _VoiceAudioComp;

    class APlayerStatePlay* _OtherPlayerState;
    bool _ClientPossesed;

    /* MENU INTERFACE */
    class AMenu3D* _MenuActor;
    void CreateMenuActor();
    bool GetOtherPlayerState();

    void TickWalkie();
    void SetLightColor();
};