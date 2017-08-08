// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameSession.h"
#include "GameSessionDedicated.generated.h"


UCLASS()
class LISTENTOMYVOICE_API AGameSessionDedicated : public AGameSession {
    GENERATED_BODY()

public:
    AGameSessionDedicated(const FObjectInitializer& OI);

    virtual void RegisterServer() override;
};
