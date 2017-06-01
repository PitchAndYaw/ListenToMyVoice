// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "MiniMap.h"

#include "GameStatePlay.h"

AMiniMap::AMiniMap(){
	_MapCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Map Camera"));
	_SceneCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture Component"));
	_SceneCaptureComp->AttachToComponent(_MapCamera, FAttachmentTransformRules::KeepRelativeTransform);
}

void AMiniMap::BeginPlay() {
	Super::BeginPlay();

}



