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

    void AfterPossessed();
    //This override is because CLIENT_AfterPossessed does not work in host (Client-server)
    UFUNCTION()
    void OnRep_Pawn() override;
    
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_CallUpdate(FPlayerInfo info);

    /*************************************** VOICE ***********************************************/
    virtual void ModifyVoiceAudioComponent(const FUniqueNetId& RemoteTalkerId,
                                           class UAudioComponent* AudioComponent) override;

    /* Radio Delegate */
    void OnRadioPressed();
    void OnRadioReleased();

    UFUNCTION(Client, Reliable)
    void CLIENT_Dead();
    UFUNCTION(Client, Reliable)
    void CLIENT_GotoState(FName NewState);

    /*************** TRIGGER MENU *************/
    void ToogleMenu();
    UFUNCTION(Client, Reliable)
    void CLIENT_ShowMenu();
    UFUNCTION(Client, Reliable)
    void CLIENT_HideMenu();

protected:
    class UNWGameInstance* _GameInstance;

    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;

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

    void TickWalkie();
    void SetLightColor();
};