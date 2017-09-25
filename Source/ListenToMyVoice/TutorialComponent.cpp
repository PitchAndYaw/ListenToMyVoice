// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "TutorialComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "NWGameInstance.h"

#include "PlayerCharacter.h"
#include "VRCharacter.h"
#include "FPCharacter.h"
#include "InventoryItem.h"

UTutorialComponent::UTutorialComponent() : Super() {
    PrimaryComponentTick.bCanEverTick = true;

    SetEnableGravity(false);
    SetCollisionProfileName("NoCollision");
    bGenerateOverlapEvents = false;
    bDrawAtDesiredSize = true;

    _WidgetSteps = {};
    _StepPivot = -1;
    _IsVR = false;
    _IsMovable = true;
    _IsDynamic = true;
}

void UTutorialComponent::BeginPlay() {
    Super::BeginPlay();

    if (GetOwner()->Role == ROLE_Authority) {// No widgets in server please
        DestroyComponent();
    }
}

void UTutorialComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (_Character) {
        if (_IsMovable) {
            FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(),
                                                                        _Character->GetPlayerCamera()->GetComponentLocation());

            SetWorldRotation(PlayerRot);
        }
        if(_IsDynamic) CheckCondition();
    }
    else CheckPlayer();
}

void UTutorialComponent::CheckPlayer() {
    if (GetWorld()) {
        if (GetWorld()->GetFirstPlayerController()) {
            UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetWorld()->GetFirstPlayerController()->GetGameInstance());
            if (GameInstance && GameInstance->_IsVRMode != _IsVR) DestroyComponent();

            if (GetWorld()->GetFirstPlayerController()->GetCharacter()) {
                _Character = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
            }
        }
    }
}

bool UTutorialComponent::CheckCondition() {
    bool Result = false;
    if (_WidgetSteps.Num() > 0) {
        if (_StepPivot < 0) NextStep();
        else {
            switch (_WidgetSteps[_StepPivot]._Condition) {
                /* COMMON */
                case ETutorialCondition::Save:
                {
                    if (_ActorRef) {
                        UInventoryItem* Item = Cast<UInventoryItem>(_ActorRef->GetComponentByClass(
                            UInventoryItem::StaticClass()));
                        if (Item && Item->IsEquipped()) { NextStep(); }
                    }
                };
                break;

                case ETutorialCondition::ItemInHand:
                {
                    if (_Character && (_Character->_ItemLeft == _ActorRef ||
                                       _Character->_ItemRight == _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                case ETutorialCondition::NoItemInHand:
                {
                    if (_Character && (_Character->_ItemLeft != _ActorRef &&
                                       _Character->_ItemRight != _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                case ETutorialCondition::UseItemPressed:
                {
                    if (_Character && (_Character->_LastItemUsedPressedLeft == _ActorRef ||
                                       _Character->_LastItemUsedPressedRight == _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                case ETutorialCondition::UseItemReleased:
                {
                    if (_Character && (_Character->_LastItemUsedReleasedLeft == _ActorRef ||
                                       _Character->_LastItemUsedReleasedRight == _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                case ETutorialCondition::UsePressed:
                {
                    if (_Character && (_Character->_LastUsedPressed == _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                case ETutorialCondition::UseReleased:
                {
                    if (_Character && (_Character->_LastUsedReleased == _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                
                /* VR */
                case ETutorialCondition::BeginGrab:
                {
                    AVRCharacter* VRCharacter = Cast<AVRCharacter>(_Character);
                    if (VRCharacter && (VRCharacter->_ActorGrabbing == _ActorRef)) { NextStep(); }
                };
                break;
                case ETutorialCondition::Drop:
                {
                    AVRCharacter* VRCharacter = Cast<AVRCharacter>(_Character);
                    if (VRCharacter && (!VRCharacter->_ItemLeft || !VRCharacter->_ItemRight)) { NextStep(); }
                };
                break;

                /* FP */
                case ETutorialCondition::OpenInv:
                {
                    AFPCharacter* FPCharacter = Cast<AFPCharacter>(_Character);
                    if (FPCharacter && !FPCharacter->_IsInventoryHidden) { NextStep(); }
                };
                break;
                case ETutorialCondition::CloseInv:
                {
                    AFPCharacter* FPCharacter = Cast<AFPCharacter>(_Character);
                    if (FPCharacter && FPCharacter->_IsInventoryHidden) { NextStep(); }
                };
                break;
                default: ;
                break;
            }
        }
    }
    return Result;
}

void UTutorialComponent::NextStep() {
    APlayerController* PC = Cast<APlayerController>(_Character->GetController());
    if (PC) {
        _StepPivot++;
        if (_StepPivot < _WidgetSteps.Num()) {
            if (_Widget) _Widget->Destruct();
            _Widget = CreateWidget<UUserWidget>(PC, _WidgetSteps[_StepPivot]._WidgetClass);

            SetWidgetClass(_WidgetSteps[_StepPivot]._WidgetClass);
            SetWidget(_Widget);
            //SetRelativeRotation(FRotator(0,0,0));
            //SetVisibility(true);
            //RegisterComponent();
        }
        else DestroyComponent();
    }
}