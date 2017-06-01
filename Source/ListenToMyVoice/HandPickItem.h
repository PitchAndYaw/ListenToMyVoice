// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "HandPickItem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LISTENTOMYVOICE_API UHandPickItem : public UActorComponent
{
	GENERATED_BODY()

public:
	UHandPickItem();

    virtual void BeginPlay() override;

};
