// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "NWGameInstance.generated.h"


class AMenu3D;
class UInputMenu;

USTRUCT(BlueprintType, Category = "Player Info")
struct FPlayerInfo {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    TSubclassOf<ACharacter> CharacterClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    bool IsHost;
};

USTRUCT(BlueprintType, Category = "Menu Options")
struct FMenuOptions {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Options")
    bool bComfortMode;
};

UCLASS()
class LISTENTOMYVOICE_API UNWGameInstance : public UGameInstance {
    GENERATED_BODY()

/**************************************** SESSION ************************************************/
public:
    bool _IsVRMode;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    FMenuOptions _MenuOptions;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    FPlayerInfo _PlayerInfoSaved;
    UPROPERTY(Replicated)
    FName _ServerName;
    UPROPERTY(Replicated)
    int _MaxPlayers;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Session")
    FString _SessionOwner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString _MapLobbyName;

    UNWGameInstance(const FObjectInitializer& OI);

    UFUNCTION(BlueprintCallable, Category = "Start")
    void InitGame();

    /*** BLUEPRINTS ***/
    UFUNCTION(BlueprintCallable, Category = "Session")
    void LaunchLobby();
    UFUNCTION(BlueprintCallable, Category = "Session")
    void FindOnlineGames();
    UFUNCTION(BlueprintCallable, Category = "Session")
    void JoinOnlineGame();
    UFUNCTION(BlueprintCallable, Category = "Session")
    void DestroySession();

    /**
    *	Function to host a game!
    *
    *	@Param	UserID - User that started the request
    *	@Param	SessionName - Name of the Session
    *	@Param	bIsLAN - Is this is LAN Game?
    *	@Param	bIsPresence - "Is the Session to create a presence Session"
    *	@Param	MaxNumPlayers - Number of Maximum allowed players on this "Session" (Server)
    */
    bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN,
                     bool bIsPresence, int32 MaxNumPlayers);

    /**
    *	Find an online session
    *
    *	@param UserId user that initiated the request
    *	@param SessionName name of session this search will generate
    *	@param bIsLAN are we searching LAN matches
    *	@param bIsPresence are we searching presence sessions
    */
    void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

    /**
    *	Joins a session via a search result
    *
    *	@param SessionName name of session
    *	@param SearchResult Session to join
    *
    *	@return bool true if successful, false otherwise
    */
    bool JoinAtSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
                       const FOnlineSessionSearchResult& SearchResult);

    /* Delegate called when session created */
    FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
    FDelegateHandle OnCreateSessionCompleteDelegateHandle;
    virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

    /* Delegate called when session started */
    FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
    FDelegateHandle OnStartSessionCompleteDelegateHandle;
    void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);


    /* Delegate for searching for sessions */
    FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
    FDelegateHandle OnFindSessionsCompleteDelegateHandle;
    void OnFindSessionsComplete(bool bWasSuccessful);

    /* Delegate for joining a session */
    FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
    FDelegateHandle OnJoinSessionCompleteDelegateHandle;
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    /* Delegate for destroying a session */
    FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
    FDelegateHandle OnDestroySessionCompleteDelegateHandle;
    virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    /* MENU INTERFACE */
    AMenu3D* CreateMenuMain();
    AMenu3D* CreateMenuLobby();
    AMenu3D* CreateMenuPlay();
    /* BINDINGS */
    void OnButtonNewGame(UInputMenu* InputMenu);
    void OnButtonOptions(UInputMenu* InputMenu);
    void OnButtonExitGame(UInputMenu* InputMenu);

    void OnButtonHostGame(UInputMenu* InputMenu);
    void OnButtonFindGame(UInputMenu* InputMenu);

    void OnButtonJoinGame(UInputMenu* InputMenu);
    void OnButtonSwitchComfortMode(UInputMenu* InputMenu);

    void OnButtonBackToMenu(UInputMenu* InputMenu);
    void OnRes640(UInputMenu* InputMenu);
    
protected:
    TSubclassOf<ACharacter> _BoyClass;
    TSubclassOf<ACharacter> _GirlClass;
    TSubclassOf<ACharacter> _VRBoyClass;
    TSubclassOf<ACharacter> _VRGirlClass;

    TSubclassOf<ACharacter> _DefaultCharacterClass;
    TSubclassOf<ACharacter> _VRDefaultCharacterClass;

    TSharedPtr<class FOnlineSessionSettings> _SessionSettings;
    TSharedPtr<class FOnlineSessionSearch> _SessionSearch;

    IOnlineSessionPtr GetSessions();

private:
    /* MENU INTERFACE */
    AMenu3D* _MenuActor;

    void CreateOptionsPanel();
    void ChangeResolution(FString Resolution);
};
