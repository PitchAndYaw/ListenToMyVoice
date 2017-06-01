// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "SoundAnimNotify.h"
#include "FMODAudioComponent.h"


USoundAnimNotify::USoundAnimNotify(const FObjectInitializer& OI) : Super(OI) {
    _Tag = "";
}

void USoundAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
    if (MeshComp->GetOwner()) {
        TArray<UActorComponent*> Components = MeshComp->GetOwner()->GetComponentsByTag(UFMODAudioComponent::StaticClass(), _Tag);
        for (UActorComponent* Component : Components) {
            UFMODAudioComponent* FMODComp = Cast<UFMODAudioComponent>(Component);
            if (FMODComp) {
                FMODComp->Play();
            }
        }
    }
}

FString USoundAnimNotify::GetNotifyName_Implementation() const {
    return _Tag.ToString();
}