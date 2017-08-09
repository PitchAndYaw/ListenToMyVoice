// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "GameSessionDedicated.h"

AGameSessionDedicated::AGameSessionDedicated(const FObjectInitializer& OI) : Super(OI) {}

void AGameSessionDedicated::RegisterServer() {
    IOnlineSessionPtr Sessions;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub) {
        Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid()) {
            FOnlineSessionSettings Settings;

            Settings.bIsLANMatch = true;
            Settings.bUsesPresence = true;
            Settings.NumPublicConnections = 2;
            Settings.NumPrivateConnections = 0;
            Settings.bAllowInvites = true;
            Settings.bAllowJoinInProgress = true;
            Settings.bShouldAdvertise = true;
            Settings.bAllowJoinViaPresence = true;
            Settings.bAllowJoinViaPresenceFriendsOnly = false;

            Sessions->CreateSession(0, GameSessionName, Settings);

            ULibraryUtils::Log("CreateSession");
        }
    }
    else ULibraryUtils::Log("No OnlineSubsytem found!");
}


void AGameSessionDedicated::RegisterPlayer(APlayerController* NewPlayer, const TSharedPtr<const FUniqueNetId>& UniqueId, bool bWasFromInvite) {
    Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
    ULibraryUtils::Log("RegisterPlayer");
}

void AGameSessionDedicated::PostLogin(APlayerController* NewPlayer) {
    Super::PostLogin(NewPlayer);
    ULibraryUtils::Log("AGameSessionDedicated::PostLogin");
}