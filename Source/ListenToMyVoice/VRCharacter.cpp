// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "VRCharacter.h"

#include "InventoryItem.h"

#include "ItfUsable.h"
#include "ItfUsableItem.h"
#include "GrabItem.h"
#include "NWGameInstance.h"

/* VR Includes */
#include "HeadMountedDisplay.h"
#include "IMotionController.h"
#include "MotionControllerComponent.h"
#include "SteamVRChaperoneComponent.h"


AVRCharacter::AVRCharacter(const FObjectInitializer& OI) : Super(OI) {
    PrimaryActorTick.bCanEverTick = true;
    bPositionalHeadTracking = true;

    _NextInventoryIndex = 0;

    /* VR TURN */
    _TurnSide = 0;
    _BaseTurnRate = 11.f;
    _TurnVelocity = 200;
    _TurnAcceleration = -15;

    /* VR MOVE */
    GetCharacterMovement()->MaxAcceleration = 512;
    GetCharacterMovement()->GroundFriction = 0.1;
    GetCharacterMovement()->BrakingDecelerationWalking = 256;

    GetCapsuleComponent()->InitCapsuleSize(34.f, 88.0f);

    static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> FFFinderLeft(
        TEXT("/Game/BluePrints/Effects/RumbleLightLeft"));
    _RumbleOverLapLeft = FFFinderLeft.Object;
    static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> FFFinderRight(
        TEXT("/Game/BluePrints/Effects/RumbleLightRight"));
    _RumbleOverLapRight = FFFinderRight.Object;
    
    _VROriginComp = CreateDefaultSubobject<USceneComponent>(TEXT("_VROriginComp"));
    _VROriginComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    _VROriginComp->RelativeLocation.Z -= 100;

    _PlayerCamera->AttachToComponent(_VROriginComp, FAttachmentTransformRules::KeepRelativeTransform);
    _PlayerCamera->PostProcessBlendWeight = 1;
    _ChaperoneComp = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("_ChaperoneComp"));

    HMD = nullptr;
    _GripStateLeft = EGripEnum::Open;
    _GripStateRight = EGripEnum::Open;

    BuildLeft();
    BuildRight();

    _RightSphere->OnComponentBeginOverlap.AddDynamic(this, &AVRCharacter::OnOverlap);
    _RightSphere->OnComponentEndOverlap.AddDynamic(this, &AVRCharacter::OnEndOverlap);
    _LeftSphere->OnComponentBeginOverlap.AddDynamic(this, &AVRCharacter::OnOverlap);
    _LeftSphere->OnComponentEndOverlap.AddDynamic(this, &AVRCharacter::OnEndOverlap);
}

void AVRCharacter::BuildLeft() {
    _LeftHandComp = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("_LeftHandComp"));
    _LeftHandComp->Hand = EControllerHand::Left;
    _LeftHandComp->AttachToComponent(_VROriginComp, FAttachmentTransformRules::KeepRelativeTransform);
    _LeftHandComp->SetRelativeLocation(FVector(10.f, 0.f, 0.f));

    /* MESH */
    _SM_LeftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("_SM_LeftHand"));
    _SM_LeftHand->AttachToComponent(_LeftHandComp, FAttachmentTransformRules::KeepRelativeTransform);
    _SM_LeftHand->SetWorldScale3D(FVector(1.0f, 1.0f, -1.0f));
    _SM_LeftHand->SetRelativeRotation(FRotator(0.f, 0.0f, 90.f));
    _SM_LeftHand->SetRelativeLocation(FVector(-10.f, 0.f, 0.f));

    /* ADDITIONAL */
    _LeftSphere = CreateDefaultSubobject<USphereComponent>(TEXT("_LeftSphere"));
    _LeftSphere->AttachToComponent(_SM_LeftHand, FAttachmentTransformRules::KeepRelativeTransform);
    _LeftSphere->SetRelativeLocation(FVector(15.f, 0.f, 0.f));
    _LeftSphere->SetSphereRadius(12.f);
}

void AVRCharacter::BuildRight() {
    _RightHandComp = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("_RightHandComp"));
    _RightHandComp->Hand = EControllerHand::Right;
    _RightHandComp->AttachToComponent(_VROriginComp, FAttachmentTransformRules::KeepRelativeTransform);
    _RightHandComp->SetRelativeLocation(FVector(10.f, 0.f, 0.f));

    /* MESH */
    _SM_RightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("_SM_RightHand"));
    _SM_RightHand->AttachToComponent(_RightHandComp, FAttachmentTransformRules::KeepRelativeTransform);
    _SM_RightHand->SetRelativeRotation(FRotator(0.f, 0.0f, 90.f));
    _SM_RightHand->SetRelativeLocation(FVector(-10.f, 0.f, 0.f));

    /* ADDITIONAL */
    _RightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("_RightSphere"));
    _RightSphere->AttachToComponent(_SM_RightHand, FAttachmentTransformRules::KeepRelativeTransform);
    _RightSphere->SetRelativeLocation(FVector(15.f, 0.f, 0.f));
    _RightSphere->SetSphereRadius(12.f);
}

void AVRCharacter::BeginPlay() {
    Super::BeginPlay();

    _TurnActualVelocity = _TurnVelocity;

    HMD = (IHeadMountedDisplay*)(GEngine->HMDDevice.Get());
    if (HMD) HMD->EnablePositionalTracking(bPositionalHeadTracking);

    _GrabDelegateLeft.BindUObject(this, &AVRCharacter::ItemGrabbedLeft);
    _GrabDelegateRight.BindUObject(this, &AVRCharacter::ItemGrabbedRight);
}

void AVRCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    /* VR TURN */
    if (_TurnSide != 0) {
        AddControllerYawInput(_TurnSide * _TurnActualVelocity * DeltaTime);
        _TurnActualVelocity += _TurnAcceleration;
        if (_TurnActualVelocity <= 0) {
            _TurnSide = 0;
            _TurnActualVelocity = _TurnVelocity;
        }
    }
}

void AVRCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInput) {
    Super::SetupPlayerInputComponent(PlayerInput);

    /* MOVEMENT */
    PlayerInput->BindAction("TurnVRLeft", IE_Pressed, this, &AVRCharacter::TurnVRLeft);
    PlayerInput->BindAction("TurnVRRight", IE_Pressed, this, &AVRCharacter::TurnVRRight);

    /* ACTIONS */
    PlayerInput->BindAction("DropLeft", IE_Pressed, this, &AVRCharacter::DropLeft);
    PlayerInput->BindAction("DropRight", IE_Pressed, this, &AVRCharacter::DropRight);

    /* VR SPECIFIC */
    PlayerInput->BindAction("ToggleTrackingSpace", IE_Pressed, this, &AVRCharacter::ToggleTrackingSpace);
    PlayerInput->BindAction("ResetHMDOrigin", IE_Pressed, this, &AVRCharacter::ResetHMDOrigin);
}

void AVRCharacter::ResetHMDOrigin() {// R
    if (HMD) HMD->ResetOrientationAndPosition();
}

void AVRCharacter::ToggleTrackingSpace() {// T
    // TODO: Fix module includes for SteamVR

    //@todo Make this safe once we can add something to the DeviceType enum.  For now, make the terrible assumption this is a SteamVR device.
    //FSteamVRHMD* SteamVRHMD = (FSteamVRHMD*)(GEngine->HMDDevice.Get());
    //if (SteamVRHMD && SteamVRHMD->IsStereoEnabled())
    //{
    // 	ESteamVRTrackingSpace TrackingSpace = SteamVRHMD->GetTrackingSpace();
    // 	SteamVRHMD->SetTrackingSpace(TrackingSpace == ESteamVRTrackingSpace::Seated ? ESteamVRTrackingSpace::Standing : ESteamVRTrackingSpace::Seated);
    //}
}

/************** OVERLAPPING *************/
void AVRCharacter::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                             bool bFromSweep, const FHitResult& SweepResult) {

    if (SweepResult.Actor.IsValid()) {
        TArray<UActorComponent*> Components;
        OtherActor->GetComponents(Components);

        UStaticMeshComponent* _StaticMesh;
        bool HitItem = false;
        for (UActorComponent* Component : Components) {
            // Highlight outline colors:
            // GREEN: 252 | BLUE: 253 | ORANGE: 254 | WHITE: 255
            if (Component->GetClass()->ImplementsInterface(UItfUsable::StaticClass())) {
                _StaticMesh = Cast<UStaticMeshComponent>(OtherActor->GetComponentByClass(
                    UStaticMeshComponent::StaticClass()));

                if (_StaticMesh) {
                    _StaticMesh->SetRenderCustomDepth(true);
                    _StaticMesh->SetCustomDepthStencilValue(252);
                    HitItem = true;
                }
            }
            else if (Component->GetClass() == UGrabItem::StaticClass()) {
                _StaticMesh = Cast<UStaticMeshComponent>(OtherActor->GetComponentByClass(
                    UStaticMeshComponent::StaticClass()));

                if (_StaticMesh) {
                    _StaticMesh->SetRenderCustomDepth(true);
                    _StaticMesh->SetCustomDepthStencilValue(255);
                    HitItem = true;
                }
            }
        }
        if (HitItem) {
            APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (OverlappedComponent == _LeftSphere) {
                _ActorFocusedLeft = OtherActor;
                if (PlayerController)
                    PlayerController->ClientPlayForceFeedback(_RumbleOverLapLeft, false, "rumble");
                SERVER_UpdateAnimation(EGripEnum::CanGrab, 1);
            }
            else if (OverlappedComponent == _RightSphere) {
                _ActorFocusedRight = OtherActor;
                if (PlayerController)
                    PlayerController->ClientPlayForceFeedback(_RumbleOverLapRight, false, "rumble");
                SERVER_UpdateAnimation(EGripEnum::CanGrab, 2);
            }
        }
    }
}

void AVRCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
    if (OtherActor == _ActorFocusedLeft) {
        _ActorFocusedLeft = nullptr;
        SERVER_UpdateAnimation(EGripEnum::Open, 1);
    }
    else if (OtherActor == _ActorFocusedRight) {
        _ActorFocusedRight = nullptr;
        SERVER_UpdateAnimation(EGripEnum::Open, 2);
    }

    UStaticMeshComponent* _StaticMesh = Cast<UStaticMeshComponent>(OtherActor->GetComponentByClass(
        UStaticMeshComponent::StaticClass()));
    if (_StaticMesh) {
        _StaticMesh->SetCustomDepthStencilValue(0);
        _StaticMesh->SetRenderCustomDepth(false);
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (_LastUsedPressed && _LastUsedPressed != _LastUsedReleased &&
        PlayerController && PlayerController->IsLocalPlayerController()) UseReleasedFocusOut();
}

/****************************************** ACTION MAPPINGS **************************************/
/*********** MOVEMENT ***********/
void AVRCharacter::MoveForward(float Value) {
    //_PlayerCamera->PostProcessSettings.bOverride_VignetteIntensity = true;

    //UNWGameInstance* GameInst = Cast<UNWGameInstance>(GetWorld()->GetGameInstance());
    //if (GameInst && GameInst->_MenuOptions.bComfortMode && Value != 0)
    //    _PlayerCamera->PostProcessSettings.VignetteIntensity = 4;
    //else _PlayerCamera->PostProcessSettings.VignetteIntensity = 0;

    AddMovementInput(_PlayerCamera->GetForwardVector(), Value);
}

void AVRCharacter::TurnVRLeft() {
    UNWGameInstance* GameInst = Cast<UNWGameInstance>(GetWorld()->GetGameInstance());
    if (GameInst && GameInst->_MenuOptions.bComfortMode) {
        AddControllerYawInput(-_BaseTurnRate);
    }
    else if (_TurnSide == 0) { _TurnSide = -1; }
}

void AVRCharacter::TurnVRRight() {
    UNWGameInstance* GameInst = Cast<UNWGameInstance>(GetWorld()->GetGameInstance());
    if (GameInst && GameInst->_MenuOptions.bComfortMode) {
        AddControllerYawInput(_BaseTurnRate);
    }
    else if (_TurnSide == 0) { _TurnSide = 1; }
}

//void AVRCharacter::TurnAtRate(float Rate) {
//    AddControllerYawInput(Rate * _BaseTurnRate * GetWorld()->GetDeltaSeconds());
//}

/******** USE ITEM LEFT *********/
void AVRCharacter::UseLeftPressed(bool IsMenuHidden) {
    if (IsMenuHidden) {
        if (_ItemLeft) {
            TArray<UActorComponent*> Components;
            _ItemLeft->GetComponents(Components);

            for (UActorComponent* Component : Components) {
                if (Component->GetClass()->ImplementsInterface(UItfUsableItem::StaticClass())) {
                    IItfUsableItem* ItfObject = Cast<IItfUsableItem>(Component);
                    if (ItfObject) {
                        _ItemPressedLeft = true;
                        ItfObject->Execute_UseItemPressed(Component);
                        _LastItemUsedPressedLeft = _ItemLeft;
                    }
                }
            }
        }
        else if (_ActorFocusedLeft) UseTriggerPressed(_ActorFocusedLeft, _SM_LeftHand, 1);
    }
    else _MenuInteractionComp->PressPointer();

    /* ANIMATION */
    SERVER_UpdateAnimation(EGripEnum::Grab, 1);
}

void AVRCharacter::UseLeftReleased(bool IsMenuHidden) {
    if (IsMenuHidden) {
        if (_ItemLeft && _LastItemUsedPressedLeft == _ItemLeft) {
            TArray<UActorComponent*> Components;
            _ItemLeft->GetComponents(Components);

            for (UActorComponent* Component : Components) {
                if (Component->GetClass()->ImplementsInterface(UItfUsableItem::StaticClass())) {
                    IItfUsableItem* ItfObject = Cast<IItfUsableItem>(Component);
                    if (ItfObject) {
                        _ItemPressedLeft = false;
                        ItfObject->Execute_UseItemReleased(Component);
                        _LastItemUsedReleasedLeft = _ItemLeft;
                    }
                }
            }
        }
        else if (_ActorFocusedLeft || _ActorGrabbing)
            UseTriggerReleased(_ActorFocusedLeft, _SM_LeftHand, 1);
    }
    else _MenuInteractionComp->ReleasePointer();

    /* ANIMATION */
    SERVER_UpdateAnimation(EGripEnum::Open, 1);
}

/******* USE ITEM RIGHT *** ******/
void AVRCharacter::UseRightPressed(bool IsMenuHidden) {
    if (IsMenuHidden) {
        if (_ItemRight) {
            TArray<UActorComponent*> Components;
            _ItemRight->GetComponents(Components);

            for (UActorComponent* Component : Components) {
                if (Component->GetClass()->ImplementsInterface(UItfUsableItem::StaticClass())) {
                    IItfUsableItem* ItfObject = Cast<IItfUsableItem>(Component);
                    if (ItfObject) {
                        _ItemPressedRight = true;
                        ItfObject->Execute_UseItemPressed(Component);
                        _LastItemUsedPressedRight = _ItemRight;
                    }
                }
            }
        }
        else if (_ActorFocusedRight) UseTriggerPressed(_ActorFocusedRight, _SM_RightHand, 2);
    }
    else _MenuInteractionComp->PressPointer();

    /* ANIMATION */
    SERVER_UpdateAnimation(EGripEnum::Grab, 2);
}

void AVRCharacter::UseRightReleased(bool IsMenuHidden) {
    if (IsMenuHidden) {
        if (_ItemRight && _LastItemUsedPressedRight == _ItemRight) {
            TArray<UActorComponent*> Components;
            _ItemRight->GetComponents(Components);

            for (UActorComponent* Component : Components) {
                if (Component->GetClass()->ImplementsInterface(UItfUsableItem::StaticClass())) {
                    IItfUsableItem* ItfObject = Cast<IItfUsableItem>(Component);
                    if (ItfObject) {
                        _ItemPressedRight = false;
                        ItfObject->Execute_UseItemReleased(Component);
                        _LastItemUsedReleasedRight = _ItemRight;
                    }
                }
            }
        }
        else if (_ActorFocusedRight || _ActorGrabbing)
            UseTriggerReleased(_ActorFocusedRight, _SM_RightHand, 2);
    }
    else _MenuInteractionComp->ReleasePointer();

    /* ANIMATION */
    SERVER_UpdateAnimation(EGripEnum::Open, 2);
}

/*************** USE TRIGGER *************/
void AVRCharacter::UseTriggerPressed(AActor* ActorFocused, USceneComponent* InParent, int Hand) {
    if (ActorFocused) {
        /* CAN BE GRABBED */
        UGrabItem* GrabItemComp = Cast<UGrabItem>(ActorFocused->GetComponentByClass(
            UGrabItem::StaticClass()));
        if (GrabItemComp) {
            /* FOCUS */
            UStaticMeshComponent* _StaticMesh = Cast<UStaticMeshComponent>(ActorFocused->GetComponentByClass(
                UStaticMeshComponent::StaticClass()));
            if (_StaticMesh) {
                _StaticMesh->SetCustomDepthStencilValue(0);
                _StaticMesh->SetRenderCustomDepth(false);
            }
            SERVER_GrabPress(ActorFocused, InParent, FName("TakeSocket"), Hand);
        }
        else {
            /* CAN BE USED */
            TArray<UActorComponent*> Components;
            ActorFocused->GetComponents(Components);
            for (UActorComponent* Component : Components) {
                if (Component->GetClass()->ImplementsInterface(UItfUsable::StaticClass())) {
                    ULibraryUtils::Log("SERVER_UsePressed");
                    SERVER_UsePressed(Component);
                }
            }
        }
    }
}

void AVRCharacter::UseTriggerReleased(AActor* ActorFocused, USceneComponent* InParent, int Hand) {
    if (_ActorGrabbing) {
        /* DROP GRABBING */
        UGrabItem* GrabItemComp = Cast<UGrabItem>(_ActorGrabbing->GetComponentByClass(
            UGrabItem::StaticClass()));
        if (GrabItemComp) {
            /* FOCUS */
            UStaticMeshComponent* _StaticMesh = Cast<UStaticMeshComponent>(_ActorGrabbing->GetComponentByClass(
                UStaticMeshComponent::StaticClass()));
            if (_StaticMesh) {
                _StaticMesh->SetCustomDepthStencilValue(0);
                _StaticMesh->SetRenderCustomDepth(false);
            }
            /* Drop item */
            SERVER_GrabRelease(Hand);
        }
    }
    else if (ActorFocused && _LastUsedPressed == ActorFocused) {
        /* CAN BE USED */
        TArray<UActorComponent*> Components;
        ActorFocused->GetComponents(Components);
        for (UActorComponent* Component : Components) {
            if (Component->GetClass()->ImplementsInterface(UItfUsable::StaticClass())) {
                ULibraryUtils::Log("SERVER_UseReleased");
                SERVER_UseReleased(Component);
            }
        }
    }
}

/********** TAKE ITEM ***********/
bool AVRCharacter::SERVER_GrabPress_Validate(AActor* Actor, USceneComponent* InParent,
                                             FName SocketName, int Hand) {
    return true;
}
void AVRCharacter::SERVER_GrabPress_Implementation(AActor* Actor, USceneComponent* InParent,
                                                   FName SocketName, int Hand) {
    MULTI_GrabPress(Actor, InParent, SocketName, Hand);
}
void AVRCharacter::MULTI_GrabPress_Implementation(AActor* Actor, USceneComponent* InParent,
                                                  FName SocketName, int Hand) {
    if (Actor) {
        _ActorGrabbing = Actor;
        UGrabItem* GrabItemComp = Cast<UGrabItem>(_ActorGrabbing->FindComponentByClass(
            UGrabItem::StaticClass()));
        if (GrabItemComp && InParent) {
            GrabItemComp->BeginGrab(InParent, SocketName);
            _ActorGrabbing->SetActorEnableCollision(false);

            if (Hand == 1) GrabItemComp->AddOnGrabDelegate(_GrabDelegateLeft);
            else if (Hand == 2) GrabItemComp->AddOnGrabDelegate(_GrabDelegateRight);
        }
    }
}

bool AVRCharacter::SERVER_GrabRelease_Validate(int Hand) {
    return true;
}
void AVRCharacter::SERVER_GrabRelease_Implementation(int Hand) {
    MULTI_GrabRelease(Hand);
}
void AVRCharacter::MULTI_GrabRelease_Implementation(int Hand) {
    if (_ActorGrabbing) {
        UGrabItem* GrabItemComp = Cast<UGrabItem>(_ActorGrabbing->FindComponentByClass(
            UGrabItem::StaticClass()));
        if (GrabItemComp) {
            GrabItemComp->EndGrab(true);
            _ActorGrabbing->SetActorEnableCollision(true);
            _ActorGrabbing = nullptr;
        }
    }
}

void AVRCharacter::ItemGrabbedLeft() {
    if (_ActorGrabbing) {
        CLIENT_AddGunDelegates(_ActorGrabbing);
        CLIENT_AddRadioDelegates(_ActorGrabbing);

        UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(_ActorGrabbing->GetComponentByClass(
            UStaticMeshComponent::StaticClass()));
        UGrabItem* GrabItemComp = Cast<UGrabItem>(_ActorGrabbing->FindComponentByClass(
            UGrabItem::StaticClass()));
        if (ItemMesh && GrabItemComp) {
            ItemMesh->SetSimulatePhysics(false);
            ItemMesh->AttachToComponent(_SM_LeftHand,
                                        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                                        TEXT("TakeSocket"));

            ItemMesh->RelativeLocation = GrabItemComp->_locationAttach_L;
            ItemMesh->RelativeRotation = GrabItemComp->_rotationAttach_L;

            _ActorGrabbing->SetActorEnableCollision(false);
            _ItemLeft = _ActorGrabbing;
            _ActorGrabbing = nullptr;
        }
    }
}

void AVRCharacter::ItemGrabbedRight() {
    if (_ActorGrabbing) {
        CLIENT_AddGunDelegates(_ActorGrabbing);
        CLIENT_AddRadioDelegates(_ActorGrabbing);

        UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(_ActorGrabbing->GetComponentByClass(
            UStaticMeshComponent::StaticClass()));
        UGrabItem* GrabItemComp = Cast<UGrabItem>(_ActorGrabbing->FindComponentByClass(
            UGrabItem::StaticClass()));
        if (ItemMesh && GrabItemComp) {
            ItemMesh->SetSimulatePhysics(false);
            ItemMesh->AttachToComponent(_SM_RightHand,
                                        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                                        TEXT("TakeSocket"));

            ItemMesh->RelativeLocation = GrabItemComp->_locationAttach_R;
            ItemMesh->RelativeRotation = GrabItemComp->_rotationAttach_R;

            _ActorGrabbing->SetActorEnableCollision(false);
            _ItemRight = _ActorGrabbing;
            _ActorGrabbing = nullptr;
        }
    }
}

void AVRCharacter::DropLeft() {
    if (_ItemLeft && !_ItemLeft->GetComponentByClass(UInventoryItem::StaticClass())) {
        SERVER_Drop(_ItemLeft, 1);
    }
    else {
        SERVER_SwitchItem(_ItemLeft, 1);
    }
}

void AVRCharacter::DropRight() {
    if (_ItemRight && !_ItemRight->GetComponentByClass(UInventoryItem::StaticClass())) {
        SERVER_Drop(_ItemRight, 2);
    }
    else {
        SERVER_SwitchItem(_ItemRight, 2);
    }
}

/********** SWITCH ITEM ***********/
bool AVRCharacter::SERVER_SwitchItem_Validate(AActor* ItemActor, int Hand) { return true; }
void AVRCharacter::SERVER_SwitchItem_Implementation(AActor* ItemActor, int Hand) {
    CLIENT_ClearGunDelegates(ItemActor);
    CLIENT_ClearRadioDelegates(ItemActor);
    MULTI_SwitchItem(ItemActor, Hand);
}
void AVRCharacter::MULTI_SwitchItem_Implementation(AActor* ItemActor, int Hand) {
    if (ItemActor) {
        UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(ItemActor->GetComponentByClass(
            UStaticMeshComponent::StaticClass()));
        if (ItemMesh) {
            ULibraryUtils::SetActorEnable(ItemActor, false);
            ItemMesh->AttachToComponent(GetRootComponent(),
                                        FAttachmentTransformRules::SnapToTargetNotIncludingScale);

            ItemMesh->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
            ItemMesh->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);

            if (Hand == 1) _ItemLeft = nullptr;
            else if (Hand == 2) _ItemRight = nullptr;

            UInventoryItem* Item = Cast<UInventoryItem>(ItemActor->GetComponentByClass(
                UInventoryItem::StaticClass()));
            if (Item) {
                _Items.AddUnique(ItemActor);
                Item->SetEquipped(true);
            }
        }
    }
    else {
        bool Found = false;
        int ItemCount = 0;
        while (!Found && ItemCount < _Items.Num()) {
            if (_Items[_NextInventoryIndex] != _ItemLeft &&
                _Items[_NextInventoryIndex] != _ItemRight) {

                Found = true;
                ULibraryUtils::SetActorEnable(_Items[_NextInventoryIndex]);
                UseTriggerPressed(_Items[_NextInventoryIndex], Hand == 1 ? _SM_LeftHand : _SM_RightHand, Hand);
            }
            ItemCount++;
            _NextInventoryIndex = ++_NextInventoryIndex % _Items.Num();
        }
    }
}

/********** UPDATE ANIMATIONS ***********/
bool AVRCharacter::SERVER_UpdateAnimation_Validate(EGripEnum NewAnim, int Hand) {
    return true;
}
void AVRCharacter::SERVER_UpdateAnimation_Implementation(EGripEnum NewAnim, int Hand) {
    MULTI_UpdateAnimation(NewAnim, Hand);
}
void AVRCharacter::MULTI_UpdateAnimation_Implementation(EGripEnum NewAnim, int Hand) {
    if (Hand == 1) {
        _GripStateLeft = NewAnim;
    }
    else if (Hand == 2) {
        _GripStateRight = NewAnim;
    }
}
