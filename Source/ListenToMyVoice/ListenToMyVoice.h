// Fill out your copyright notice in the Description page of Project Settings.

#ifndef __LTMV_H__
#define __LTMV_H__

#include "Engine.h"
#include "UnrealNetwork.h"
#include "Online.h"
#include "LibraryUtils.h"
#include "Settings.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"

#define SETTING_SESSION_NAME FName(TEXT("SESSION_NAME"))

DECLARE_DELEGATE(FRadioDelegate);
DECLARE_DELEGATE(FGrabDelegate);
DECLARE_DELEGATE_TwoParams(FGunDelegate, TSubclassOf<AActor>, const FTransform);

#endif