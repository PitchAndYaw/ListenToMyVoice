// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "NWGameInstance.generated.h"


class AMenu3D;
class UInputMenu;

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Session")
    FString _SessionOwner;

    UNWGameInstance(const FObjectInitializer& OI);

    UFUNCTION(BlueprintCallable, Category = "Start")
    void InitGame();

    /*** BLUEPRINTS ***/
    UFUNCTION(BlueprintCallable, Category = "Session")
    void FindOnlineGames();
    UFUNCTION(BlueprintCallable, Category = "Session")
    void JoinOnlineGame(int Index);
    UFUNCTION(BlueprintCallable, Category = "Session")
    void DestroySession();


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
    void OnButtonDestroyExitGame(UInputMenu* InputMenu);
    void OnButtonExitGame(UInputMenu* InputMenu);

    void OnButtonFindGame(UInputMenu* InputMenu);

    void OnButtonJoinGame(UInputMenu* InputMenu);
    void OnButtonSwitchComfortMode(UInputMenu* InputMenu);

    void OnButtonBackToMenu(UInputMenu* InputMenu);
    
    void OnButtonGraphics(UInputMenu* InputMenu);
    void OnRes1920(UInputMenu* InputMenu);
    void OnRes1600(UInputMenu* InputMenu);
    void OnRes1280(UInputMenu* InputMenu);
    void OnRes800(UInputMenu* InputMenu);
    
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
    bool _Exit;

    /* MENU INTERFACE */
    AMenu3D* _MenuActor;

    void CreateOptionsPanel();
    bool FillMenuFindGame();
    void ChangeResolution(FString Resolution);
};
