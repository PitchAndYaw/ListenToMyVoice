// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerCharacter.h"
#include "MotionControllerComponent.h"
#include "VRCharacter.generated.h"

class UGrabItem;

UENUM(BlueprintType)
enum class EGripEnum : uint8 {
    Open 	UMETA(DisplayName = "Open"),
    CanGrab UMETA(DisplayName = "CanGrab"),
    Grab	UMETA(DisplayName = "Grab")
};

UCLASS()
class LISTENTOMYVOICE_API AVRCharacter : public APlayerCharacter {
    GENERATED_BODY()

public:
    /* VR TURN */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn VR Comfort")
    float _BaseTurnRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn VR")
    float _TurnVelocity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn VR")
    float _TurnAcceleration;

    AActor* _ActorGrabbing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rumble")
    UForceFeedbackEffect * _RumbleOverLapLeft;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rumble")
    UForceFeedbackEffect * _RumbleOverLapRight;

    UPROPERTY(BlueprintReadOnly)
    EGripEnum _GripStateLeft;
    UPROPERTY(BlueprintReadOnly)
    EGripEnum _GripStateRight;

    AVRCharacter(const FObjectInitializer& OI);
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInput) override;
    virtual void Tick(float DeltaTime) override;

    void ResetHMDOrigin();

    /* Toggle between Seated and Standing VR Tracking */
    void ToggleTrackingSpace();

    /******** USE ITEM LEFT *********/
    void UseLeftPressed(bool IsMenuHidden) override;
    void UseLeftReleased(bool IsMenuHidden) override;

    /******* USE ITEM RIGHT *********/
    void UseRightPressed(bool IsMenuHidden) override;
    void UseRightReleased(bool IsMenuHidden) override;

    UMotionControllerComponent* GetControllerByHand(EControllerHand Hand);

protected:
    int _NextInventoryIndex;
    TArray<AActor*> _Items;

    UPROPERTY(EditDefaultsOnly, Category = "VR")
    bool bPositionalHeadTracking;

    /************************************* MAIN COMPONENTS ***************************************/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* _VROriginComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USteamVRChaperoneComponent* _ChaperoneComp;

    /*********************************** MOTION CONTROLLERS **************************************/
    /************ LEFT ***********/
    /* Motion Controllers */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UMotionControllerComponent* _LeftHandComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* _SM_LeftHand;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* _LeftSphere;
    /*********** RIGHT ***********/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UMotionControllerComponent* _RightHandComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* _SM_RightHand;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* _RightSphere;

    /*************** USE TRIGGER *************/
    void UseTriggerPressed(AActor* ActorFocused, USceneComponent* InParent, int Hand);
    void UseTriggerReleased(AActor* ActorFocused, USceneComponent* InParent, int Hand);

    /********** TAKE ITEM ***********/
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_GrabPress(AActor* Actor, USceneComponent* InParent, FName SocketName, int Hand);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_GrabPress(AActor* Actor, USceneComponent* InParent, FName SocketName, int Hand);

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_GrabRelease(int Hand);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_GrabRelease(int Hand);

    /********** DROP ITEM ***********/
    UFUNCTION()
    void DropLeft();
    UFUNCTION()
    void DropRight();

    /********** SWITCH ITEM ***********/
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_SwitchItem(AActor* ItemActor, int Hand);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_SwitchItem(AActor* ItemActor, int Hand);


    /*********** MOVEMENT ***********/
    void MoveForward(float Value) override;
    void TurnVRLeft();
    void TurnVRRight();
    //void TurnAtRate(float Rate);

    /********** UPDATE ANIMATIONS ***********/
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_UpdateAnimation(EGripEnum NewAnim, int Hand);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_UpdateAnimation(EGripEnum NewAnim, int Hand);

private:
    IHeadMountedDisplay* HMD;

    AActor* _ActorFocusedLeft;
    AActor* _ActorFocusedRight;

    UStaticMeshComponent* _LastMeshFocusedLeft = nullptr;
    UStaticMeshComponent* _LastMeshFocusedRight = nullptr;

    /* VR TURN */
    int _TurnSide;
    float _TurnActualVelocity;

    void BuildLeft();
    void BuildRight();

    /*** OVERLAPPING ***/
    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                   bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    FGrabDelegate _GrabDelegateLeft;
    FGrabDelegate _GrabDelegateRight;

    void ItemGrabbedLeft();
    void ItemGrabbedRight();
};
