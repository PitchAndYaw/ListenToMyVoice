// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/SceneCapture2D.h"
#include "MiniMap.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class LISTENTOMYVOICE_API AMiniMap : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();

	UPROPERTY(Category = "Diary", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* _MapCamera;
	UPROPERTY(Category = "Diary", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* _SceneCaptureComp;


public:
	AMiniMap();
};
