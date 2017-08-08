// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "GameSessionDedicated.h"

AGameSessionDedicated::AGameSessionDedicated(const FObjectInitializer& OI) : Super(OI) {}

void AGameSessionDedicated::RegisterServer() {
    ULibraryUtils::Log("RegisterServer");
    IOnlineSessionPtr Sessions;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub) {
        Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid()) {
            FOnlineSessionSettings Settings;
            Settings.NumPublicConnections = 3;
            Settings.bShouldAdvertise = true;
            Settings.bAllowJoinInProgress = true;
            Settings.bIsLANMatch = true;
            Settings.bUsesPresence = true;
            Settings.bAllowJoinViaPresence = true;

            Sessions->CreateSession(0, GameSessionName, Settings);

            ULibraryUtils::Log("CreateSession");
        }
    }
    else {
        ULibraryUtils::Log("No OnlineSubsytem found!");
    }
    return;
}