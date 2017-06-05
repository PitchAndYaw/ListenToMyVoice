// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "TutorialText.h"

#include "Kismet/KismetMathLibrary.h"


UTutorialText::UTutorialText() : Super() {
    PrimaryComponentTick.bCanEverTick = true;

    RelativeRotation = FRotator(0, -120, 0);
}

void UTutorialText::BeginPlay() {
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

void UTutorialText::TickComponent(float DeltaTime, ELevelTick TickType,
                                  FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (_Character) {
        FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(),
                                                                    _Character->GetActorLocation());
        
        SetRelativeRotation(PlayerRot);
        ULibraryUtils::Log(RelativeRotation.ToString());
    }
}