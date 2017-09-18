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

    /* Delegate called when session created */
    FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
    FDelegateHandle OnCreateSessionCompleteDelegateHandle;
    virtual void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);

    /* Delegate called when session started */
    FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
    FDelegateHandle OnStartSessionCompleteDelegateHandle;
    void OnStartOnlineGameComplete(FName InSessionName, bool bWasSuccessful);

    /* Delegate for destroying a session */
    FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
    FDelegateHandle OnDestroySessionCompleteDelegateHandle;
    virtual void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString _MapLobbyName;
};
