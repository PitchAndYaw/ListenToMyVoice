// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "TutorialComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "NWGameInstance.h"

#include "PlayerControllerLobby.h"
#include "VRCharacter.h"
#include "FPCharacter.h"
#include "InventoryItem.h"

UTutorialComponent::UTutorialComponent() : Super() {
    PrimaryComponentTick.bCanEverTick = true;

    _WidgetSteps = {};
    _StepPivot = -1;
    _IsVR = false;
}

void UTutorialComponent::BeginPlay() {
    Super::BeginPlay();

    if (GetWorld()) {
        if (GetWorld()->GetFirstPlayerController()) {
            UNWGameInstance* GameInstance = Cast<UNWGameInstance>(GetWorld()->GetFirstPlayerController()->GetGameInstance());
            if (GameInstance && GameInstance->_IsVRMode != _IsVR) DestroyComponent();

            if (GetWorld()->GetFirstPlayerController()->GetCharacter()) {
                _Character = GetWorld()->GetFirstPlayerController()->GetCharacter();
            }
        }
    }
}

void UTutorialComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (_Character) {
        FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(),
                                                                    _Character->GetActorLocation());
        
        SetRelativeRotation(PlayerRot);

        CheckCondition();
    }
}

bool UTutorialComponent::CheckCondition() {
    bool Result = false;
    if (_WidgetSteps.Num() > 0) {
        if (_StepPivot < 0) NextStep();
        else {
            switch (_WidgetSteps[_StepPivot]._Condition) {
                case ETutorialCondition::Grab:
                {
                    AVRCharacter* VRCharacter = Cast<AVRCharacter>(_Character);
                    if (VRCharacter && (VRCharacter->_ItemLeft == _ActorRef ||
                                        VRCharacter->_ItemRight == _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                case ETutorialCondition::UseItemVR:
                {
                    AVRCharacter* VRCharacter = Cast<AVRCharacter>(_Character);
                    if (VRCharacter) {
                        APlayerControllerLobby* PC = Cast<APlayerControllerLobby>(VRCharacter->GetController());
                        if (PC) {
                            if (VRCharacter && ((VRCharacter->_ItemLeft == _ActorRef && PC->_IsLefReleased) ||
                                (VRCharacter->_ItemRight == _ActorRef && PC->_IsRightReleased))) {

                                PC->_IsLefReleased = false;
                                PC->_IsRightReleased = false;
                                NextStep();
                            }
                        }
                    }
                };
                break;
                case ETutorialCondition::Drop:
                {
                    AVRCharacter* VRCharacter = Cast<AVRCharacter>(_Character);
                    if (VRCharacter && (!VRCharacter->_ItemLeft || !VRCharacter->_ItemRight)) { NextStep(); }
                };
                break;
                case ETutorialCondition::Save:
                {
                    UInventoryItem* Item = Cast<UInventoryItem>(_ActorRef->GetComponentByClass(
                        UInventoryItem::StaticClass()));
                    if (Item && Item->_equipped) { NextStep(); }
                };
                break;
                case ETutorialCondition::OpenInv:
                {
                    AFPCharacter* FPCharacter = Cast<AFPCharacter>(_Character);
                    if (FPCharacter && !FPCharacter->_IsInventoryHidden) { NextStep(); }
                };
                break;
                case ETutorialCondition::EquipItem:
                {
                    AFPCharacter* FPCharacter = Cast<AFPCharacter>(_Character);
                    if (FPCharacter && (FPCharacter->_ItemLeft == _ActorRef ||
                                        FPCharacter->_ItemRight == _ActorRef)) {
                        NextStep();
                    }
                };
                break;
                case ETutorialCondition::CloseInv:
                {
                    AFPCharacter* FPCharacter = Cast<AFPCharacter>(_Character);
                    if (FPCharacter && FPCharacter->_IsInventoryHidden) { NextStep(); }
                };
                break;
                case ETutorialCondition::UseItemFP:
                {
                    AFPCharacter* FPCharacter = Cast<AFPCharacter>(_Character);
                    if (FPCharacter) {
                        APlayerControllerLobby* PC = Cast<APlayerControllerLobby>(FPCharacter->GetController());
                        if (PC) {
                            if (FPCharacter && ((FPCharacter->_ItemLeft == _ActorRef && PC->_IsLefReleased) ||
                                (FPCharacter->_ItemRight == _ActorRef && PC->_IsRightReleased))) {

                                PC->_IsLefReleased = false;
                                PC->_IsRightReleased = false;
                                NextStep();
                            }
                        }
                    }
                };
                break;
                default:;
                    break;
            }
        }
    }
    return Result;
}

void UTutorialComponent::NextStep() {
    ULibraryUtils::Log("NextStep ");

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
        else  DestroyComponent();
    }
}