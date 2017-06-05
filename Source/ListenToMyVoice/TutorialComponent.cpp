// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "TutorialComponent.h"

#include "Kismet/KismetMathLibrary.h"


UTutorialComponent::UTutorialComponent() : Super() {
    PrimaryComponentTick.bCanEverTick = true;

    RelativeRotation = FRotator(0, -120, 0);
    _WidgetSteps = {};
    _StepPivot = 0;
}

void UTutorialComponent::BeginPlay() {
    Super::BeginPlay();

    if (GetWorld()) {
        if (GetWorld()->GetFirstPlayerController()) {
            if (GetWorld()->GetFirstPlayerController()->GetCharacter()) {
                _Character = GetWorld()->GetFirstPlayerController()->GetCharacter();
                DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
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
    }
}

void UTutorialComponent::NextStep() {
    if (_StepPivot < _WidgetSteps.Num()) {
        _StepPivot++;
        SetWidgetClass(_WidgetSteps[_StepPivot]);
    }
    else  DestroyComponent();
}