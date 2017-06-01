// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Settings.generated.h"


UCLASS(config=Game, defaultconfig, meta=(DisplayName="Settings"))
class LISTENTOMYVOICE_API USettings : public UDeveloperSettings {
    GENERATED_BODY()

public:
    UPROPERTY(config, EditAnywhere, Category="Maps", meta=(AllowedClasses="World"))
    FStringAssetReference LevelToPlay;
    UPROPERTY(config, EditAnywhere, Category = "Maps", meta = (AllowedClasses = "World"))
    FStringAssetReference LevelLobby;

    USettings(const FObjectInitializer& OI);

    static USettings* Get() { 
        return CastChecked<USettings>(USettings::StaticClass()->GetDefaultObject());
    }
};
