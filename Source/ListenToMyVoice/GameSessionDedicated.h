// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameSession.h"
#include "GameSessionDedicated.generated.h"


UCLASS()
class LISTENTOMYVOICE_API AGameSessionDedicated : public AGameSession {
    GENERATED_BODY()

public:
    AGameSessionDedicated(const FObjectInitializer& OI);

    void RegisterServer() override;
    //void RegisterPlayer(APlayerController* NewPlayer,
    //                            const TSharedPtr<const FUniqueNetId>& UniqueId,
    //                            bool bWasFromInvite) override;
    //void PostLogin(APlayerController* NewPlayer) override;

    void UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) override;
};
