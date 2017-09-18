// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "GameSessionDedicated.h"

AGameSessionDedicated::AGameSessionDedicated(const FObjectInitializer& OI) : Super(OI) {
    /** Bind function for CREATING a Session */
    OnCreateSessionCompleteDelegate =
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &AGameSessionDedicated::OnCreateSessionComplete);
    OnStartSessionCompleteDelegate =
        FOnStartSessionCompleteDelegate::CreateUObject(this, &AGameSessionDedicated::OnStartOnlineGameComplete);

    /** Bind function for DESTROYING a Session */
    OnDestroySessionCompleteDelegate =
        FOnDestroySessionCompleteDelegate::CreateUObject(this, &AGameSessionDedicated::OnDestroySessionComplete);

    _MapLobbyName = USettings::Get()->LevelLobby.GetAssetName();
}

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
            Settings.Set(SETTING_MAPNAME, _MapLobbyName, EOnlineDataAdvertisementType::ViaOnlineService);

            OnCreateSessionCompleteDelegateHandle =
                Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
            Sessions->CreateSession(0, GameSessionName, Settings);

            ULibraryUtils::Log("CreateSession");
        }
    }
    else ULibraryUtils::Log("No OnlineSubsytem found!");
}

//void AGameSessionDedicated::RegisterPlayer(APlayerController* NewPlayer, const TSharedPtr<const FUniqueNetId>& UniqueId, bool bWasFromInvite) {
//    Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
//    ULibraryUtils::Log("RegisterPlayer");
//}
//
//void AGameSessionDedicated::PostLogin(APlayerController* NewPlayer) {
//    Super::PostLogin(NewPlayer);
//    ULibraryUtils::Log("AGameSessionDedicated::PostLogin");
//}

void AGameSessionDedicated::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) {
    Super::UnregisterPlayer(InSessionName, UniqueId);

    IOnlineSessionPtr Sessions;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub) {
        Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid()) {
            FNamedOnlineSession* Session = Sessions->GetNamedSession(InSessionName);
            
            if (Session->RegisteredPlayers.Num() == 0) {
                ULibraryUtils::Log("NO PLAYERS");
                Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
                Sessions->DestroySession(GameSessionName);
            }
        }
    }
    else ULibraryUtils::Log("No OnlineSubsytem found!");
}

void AGameSessionDedicated::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful) {
    IOnlineSessionPtr Sessions;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub) {
        Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid()) {
            Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
            OnStartSessionCompleteDelegateHandle =
                Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
            Sessions->StartSession(GameSessionName);
        }
    }
}

void AGameSessionDedicated::OnStartOnlineGameComplete(FName InSessionName, bool bWasSuccessful) {
    IOnlineSessionPtr Sessions;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub) {
        Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid()) {
            Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
            UGameplayStatics::OpenLevel(GetWorld(), FName(*_MapLobbyName), true, "listen");
            ULibraryUtils::Log("StartOnlineGameComplete");
        }
    }
}

void AGameSessionDedicated::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful) {
    IOnlineSessionPtr Sessions;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub) {
        Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid()) {
            Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
            if (bWasSuccessful) {
                ULibraryUtils::Log("AGameSessionDedicated::OnDestroySessionComplete");
                RegisterServer();
            }
        }
    }
}