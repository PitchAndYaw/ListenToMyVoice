// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "SoundAnimNotify.generated.h"


UCLASS()
class LISTENTOMYVOICE_API USoundAnimNotify : public UAnimNotify {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "AnimNotify")
    FName _Tag;

    USoundAnimNotify(const FObjectInitializer& OI);

    // Begin UAnimNotify interface
    virtual FString GetNotifyName_Implementation() const override;
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    // End UAnimNotify interface
};
