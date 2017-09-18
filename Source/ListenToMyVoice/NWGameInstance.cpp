// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "NWGameInstance.h"

#include "PlayerControllerLobby.h"
#include "Menu3D.h"
#include "InputMenu.h"

/* VR Includes */
#include "HeadMountedDisplay.h"


UNWGameInstance::UNWGameInstance(const FObjectInitializer& OI) : Super(OI) {
    /** Bind function for FINDING a Session */
    OnFindSessionsCompleteDelegate =
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnFindSessionsComplete);

    /** Bind function for JOINING a Session */
    OnJoinSessionCompleteDelegate =
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnJoinSessionComplete);

    /** Bind function for DESTROYING a Session */
    OnDestroySessionCompleteDelegate =
        FOnDestroySessionCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnDestroySessionComplete);

    _SessionOwner = "";
    _IsVRMode = false;
    _Exit = false;

    static ConstructorHelpers::FClassFinder<AActor> BoyClassFinder(TEXT(
        "/Game/BluePrints/Characters/FPCharacterBoy"));
    _BoyClass = BoyClassFinder.Class;
    static ConstructorHelpers::FClassFinder<AActor> GirlClassFinder(TEXT(
        "/Game/BluePrints/Characters/FPCharacterGirl"));
    _GirlClass = GirlClassFinder.Class;

    static ConstructorHelpers::FClassFinder<AActor> VRBoyClassFinder(TEXT(
        "/Game/BluePrints/Characters/VRCharacterBoy"));
    _VRBoyClass = VRBoyClassFinder.Class;
    static ConstructorHelpers::FClassFinder<AActor> VRGirlClassFinder(TEXT(
        "/Game/BluePrints/Characters/VRCharacterGirl"));
    _VRGirlClass = VRGirlClassFinder.Class;

    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT(
        "/Game/BluePrints/Characters/FPCharacter_BP"));
    _DefaultCharacterClass = PlayerPawnClassFinder.Class;

    static ConstructorHelpers::FClassFinder<APawn> PlayerVRPawnClassFinder(TEXT(
        "/Game/BluePrints/Characters/VRCharacter_BP"));
    _VRDefaultCharacterClass = PlayerVRPawnClassFinder.Class;

    _MenuOptions.bComfortMode = false;
}

IOnlineSessionPtr UNWGameInstance::GetSessions() {
    IOnlineSessionPtr Sessions;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub) {
        Sessions = OnlineSub->GetSessionInterface();
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
    }
    return Sessions;
}

void UNWGameInstance::InitGame() {
    ULibraryUtils::Log("InitGame");
    /* SWITCH PLAYER MODE */
    if (FParse::Param(FCommandLine::Get(), TEXT("vr"))) _IsVRMode = true;

    if (GEngine) {
        IHeadMountedDisplay* HMD = (IHeadMountedDisplay*)(GEngine->HMDDevice.Get());
        if (HMD) {
            HMD->EnableHMD(_IsVRMode);
            HMD->EnableStereo(_IsVRMode);
        }
    }

    APlayerControllerLobby* const PC = Cast<APlayerControllerLobby>(GetFirstLocalPlayerController());
    AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
    if (PC && GameMode) {
        TSubclassOf<ACharacter> CharacterClass = _IsVRMode ? _VRDefaultCharacterClass :
            _DefaultCharacterClass;
        FTransform Transform = GameMode->FindPlayerStart(PC, TEXT("playerstart"))
            ->GetActorTransform();
        APawn* Actor = Cast<APawn>(GetWorld()->SpawnActor(CharacterClass, &Transform));
        if (Actor) {
            PC->Possess(Actor);
            PC->CLIENT_CreateMenu();
        }
    }
}

/**************************************** BLUEPRINTS *********************************************/
void UNWGameInstance::FindOnlineGames() {
    ULocalPlayer* const Player = GetFirstGamePlayer();
    FindSessions(Player->GetPreferredUniqueNetId(), true, true);
}

void UNWGameInstance::JoinOnlineGame(int Index) {
    ULocalPlayer* const Player = GetFirstGamePlayer();
    FOnlineSessionSearchResult SearchResult;
    if (Index < _SessionSearch->SearchResults.Num()) {
        SearchResult = _SessionSearch->SearchResults[Index];
        if (SearchResult.Session.OwningUserId != Player->GetPreferredUniqueNetId()) {
            JoinAtSession(Player->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
        }
    }
}

void UNWGameInstance::DestroySession() {
    IOnlineSessionPtr Sessions = GetSessions();
    if (Sessions.IsValid()) {
        OnDestroySessionCompleteDelegateHandle =
            Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

        ULibraryUtils::Log("DestroySession");
        Sessions->DestroySession(GameSessionName);
    }
}

/**************************************** SESSION ************************************************/
void UNWGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence) {
    IOnlineSessionPtr Sessions = GetSessions();
    if (Sessions.IsValid() && UserId.IsValid()) {
        _SessionSearch = MakeShareable(new FOnlineSessionSearch());
        _SessionSearch->bIsLanQuery = bIsLAN;
        _SessionSearch->MaxSearchResults = 20;
        _SessionSearch->PingBucketSize = 50;
        _SessionSearch->TimeoutInSeconds = 15;

        TSharedRef<FOnlineSessionSearch> SearchSettingsRef = _SessionSearch.ToSharedRef();
        OnFindSessionsCompleteDelegateHandle =
            Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
        _SessionOwner = "";
        Sessions->FindSessions(*UserId, SearchSettingsRef);
    }
}

void UNWGameInstance::OnFindSessionsComplete(bool bWasSuccessful) {
    ULibraryUtils::Log(FString::Printf(TEXT("Number of Sessions found: %d"),
                                       _SessionSearch->SearchResults.Num()), 3, 15);
    IOnlineSessionPtr Sessions = GetSessions();
    bool Ok = false;
    if (Sessions.IsValid()) {
        Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
        Ok = FillMenuFindGame();
    }

    int Index = _MenuActor->GetSubmenuNum() - 1;
    _MenuActor->SetSubmenuByIndex(Index);
    if (Ok) _MenuActor->SetInputMenuLoading(Index, 0, false, "");
    else _MenuActor->SetInputMenuLoading(Index, 0, false, "NO GAMES");

    _MenuActor->PlayEndFindSessions(Ok);
}

bool UNWGameInstance::JoinAtSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
                                    const FOnlineSessionSearchResult& SearchResult) {
    bool bSuccessful = false;
    IOnlineSessionPtr Sessions = GetSessions();
    if (Sessions.IsValid() && UserId.IsValid()) {
        OnJoinSessionCompleteDelegateHandle =
            Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
        bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
    }
    return bSuccessful;
}

void UNWGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
    IOnlineSessionPtr Sessions = GetSessions();
    if (Sessions.IsValid()) {
        Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
        APlayerController * const PlayerController = GetFirstLocalPlayerController();
        FString TravelURL;
        if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL)) {
            PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
        }
    }
}

void UNWGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) {
    IOnlineSessionPtr Sessions = GetSessions();
    if (Sessions.IsValid()) {
        Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
        if (bWasSuccessful) {
            ULibraryUtils::Log("UNWGameInstance::OnDestroySessionComplete");
            if (_Exit) {
                _Exit = false;
                FGenericPlatformMisc::RequestExit(false);
            }
            else {
                APlayerController * const PlayerController = GetFirstLocalPlayerController();
                if (PlayerController) PlayerController->ClientReturnToMainMenu("");
            }
        }
    }
}

/*********************************** MENU INTERFACE **********************************************/
AMenu3D* UNWGameInstance::CreateMenuMain() {
    if (ULibraryUtils::IsValid(_MenuActor)) _MenuActor->Destroy();

    _MenuActor = GetWorld()->SpawnActor<AMenu3D>();

    /*** (0)MAIN MENU ***/
    UMenuPanel* MenuMain = NewObject<UMenuPanel>(_MenuActor, FName("MenuMain"));
    UInputMenu* Slot_NewGame = NewObject<UInputMenu>(_MenuActor, FName("NEW GAME"));
    Slot_NewGame->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonNewGame);
    Slot_NewGame->AddOnInputMenuDelegate();
    UInputMenu* Slot_Options = NewObject<UInputMenu>(_MenuActor, FName("OPTIONS"));
    Slot_Options->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonOptions);
    Slot_Options->AddOnInputMenuDelegate();
    UInputMenu* Slot_ExitGame = NewObject<UInputMenu>(_MenuActor, FName("EXIT GAME"));
    Slot_ExitGame->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonExitGame);
    Slot_ExitGame->AddOnInputMenuDelegate();

    _MenuActor->AddSubmenu(MenuMain);
    MenuMain->AddMenuInput(Slot_NewGame);
    MenuMain->AddMenuInput(Slot_Options);
    MenuMain->AddMenuInput(Slot_ExitGame);

    /*** OPTIONS ***/
    CreateOptionsPanel();

    /*** (TOTAL -2)NEW GAME MENU ***/
    UMenuPanel* MenuNewGame = NewObject<UMenuPanel>(_MenuActor, FName("MenuNewGame"));
    UInputMenu* Slot_FindGame = NewObject<UInputMenu>(_MenuActor, FName("FIND GAME"));
    Slot_FindGame->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonFindGame);
    Slot_FindGame->AddOnInputMenuDelegate();

    _MenuActor->AddSubmenu(MenuNewGame);
    MenuNewGame->AddMenuInput(Slot_FindGame);

    /*** (TOTAL - 1)FIND GAME MENU ***/
    UMenuPanel* MenuFindGame = NewObject<UMenuPanel>(_MenuActor, FName("MenuFindGame"));
    UInputMenu* Slot_JoinGame = NewObject<UInputMenu>(_MenuActor, FName("JOIN GAME"));
    //Slot_JoinGame->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonJoinGame);
    //Slot_JoinGame->AddOnInputMenuDelegate();

    _MenuActor->AddSubmenu(MenuFindGame);
    MenuFindGame->AddMenuInput(Slot_JoinGame);

    return _MenuActor;
}

AMenu3D* UNWGameInstance::CreateMenuLobby() {
    return CreateMenuPlay();
}

AMenu3D* UNWGameInstance::CreateMenuPlay() {
    if (ULibraryUtils::IsValid(_MenuActor)) _MenuActor->Destroy();

    _MenuActor = GetWorld()->SpawnActor<AMenu3D>();

    /*** (0)PLAY MENU ***/
    UMenuPanel* MenuPlay = NewObject<UMenuPanel>(_MenuActor, FName("MenuPlay"));
    UInputMenu* Slot_BackToMenu = NewObject<UInputMenu>(_MenuActor, FName("BACK TO MENU"));
    Slot_BackToMenu->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonBackToMenu);
    Slot_BackToMenu->AddOnInputMenuDelegate();
    UInputMenu* Slot_Options = NewObject<UInputMenu>(_MenuActor, FName("OPTIONS"));
    Slot_Options->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonOptions);
    Slot_Options->AddOnInputMenuDelegate();
    UInputMenu* Slot_ExitGame = NewObject<UInputMenu>(_MenuActor, FName("EXIT GAME"));
    Slot_ExitGame->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonDestroyExitGame);
    Slot_ExitGame->AddOnInputMenuDelegate();

    _MenuActor->AddSubmenu(MenuPlay);
    MenuPlay->AddMenuInput(Slot_BackToMenu);
    MenuPlay->AddMenuInput(Slot_Options);
    MenuPlay->AddMenuInput(Slot_ExitGame);

    /*** OPTIONS ***/
    CreateOptionsPanel();

    return _MenuActor;
}

void UNWGameInstance::CreateOptionsPanel() {
    /*** (1)OPTIONS MENU ***/
    UMenuPanel* MenuOptions = NewObject<UMenuPanel>(_MenuActor, FName("MenuOptions"));
    _MenuActor->AddSubmenu(MenuOptions);
    if (_IsVRMode) {
        UInputMenu* Slot_ComfortMode = NewObject<UInputMenu>(_MenuActor,
                                                             _MenuOptions.bComfortMode ? "COMFORT ON" : "COMFORT OFF");
        Slot_ComfortMode->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonSwitchComfortMode);
        Slot_ComfortMode->AddOnInputMenuDelegate();

        MenuOptions->AddMenuInput(Slot_ComfortMode);
    }
    else {
        UInputMenu* Slot_Graphics = NewObject<UInputMenu>(_MenuActor, "GRAPHICS");
        Slot_Graphics->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonGraphics);
        Slot_Graphics->AddOnInputMenuDelegate();

        MenuOptions->AddMenuInput(Slot_Graphics);

        /*** (2)GRAPHICS MENU ***/
        UMenuPanel* MenuGraphics = NewObject<UMenuPanel>(_MenuActor, FName("MenuGraphics"));
        UInputMenu* Slot_Res1920 = NewObject<UInputMenu>(_MenuActor, "1920x1080");
        Slot_Res1920->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnRes1920);
        Slot_Res1920->AddOnInputMenuDelegate();
        UInputMenu* Slot_Res1600 = NewObject<UInputMenu>(_MenuActor, "1600x900");
        Slot_Res1600->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnRes1600);
        Slot_Res1600->AddOnInputMenuDelegate();
        UInputMenu* Slot_Res1280 = NewObject<UInputMenu>(_MenuActor, "1280x720");
        Slot_Res1280->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnRes1280);
        Slot_Res1280->AddOnInputMenuDelegate();
        UInputMenu* Slot_Res800 = NewObject<UInputMenu>(_MenuActor, "800x600");
        Slot_Res800->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnRes800);
        Slot_Res800->AddOnInputMenuDelegate();

        _MenuActor->AddSubmenu(MenuGraphics);
        MenuGraphics->AddMenuInput(Slot_Res1920);
        MenuGraphics->AddMenuInput(Slot_Res1600);
        MenuGraphics->AddMenuInput(Slot_Res1280);
        MenuGraphics->AddMenuInput(Slot_Res800);
    }
}

bool UNWGameInstance::FillMenuFindGame() {
    int Index = _MenuActor->GetSubmenuNum() - 1;
    bool Ok = false;
    if (_SessionSearch.IsValid()) {
        FString Result = "";
        for (int32 i = 0; i < _SessionSearch->SearchResults.Num(); i++) {
            if (_SessionSearch->SearchResults[i].Session.NumOpenPublicConnections > 0) {
                Result = _SessionSearch->SearchResults[i].Session.OwningUserName;
                Ok = true;

                UInputMenu* Slot_GameSlot = NewObject<UInputMenu>(_MenuActor, FName(*Result));
                Slot_GameSlot->_InputMenuReleasedDelegate.BindUObject(this, &UNWGameInstance::OnButtonJoinGame);
                Slot_GameSlot->AddOnInputMenuDelegate();

                _MenuActor->GetSubmenu(Index)->AddMenuInput(Slot_GameSlot);
            }
        }
    }
    return Ok;
}

/*********************************** BINDINGS ****************************************************/
void UNWGameInstance::OnButtonNewGame(UInputMenu* InputMenu) {
    _MenuActor->SetSubmenuByIndex(_MenuActor->GetSubmenuNum() - 2);
}

void UNWGameInstance::OnButtonOptions(UInputMenu* InputMenu) {
    _MenuActor->SetSubmenuByIndex(1);
}

void UNWGameInstance::OnButtonDestroyExitGame(UInputMenu* InputMenu) {
    _Exit = true;
    DestroySession();
}

void UNWGameInstance::OnButtonExitGame(UInputMenu* InputMenu) {
    FGenericPlatformMisc::RequestExit(false);
}

void UNWGameInstance::OnButtonFindGame(UInputMenu* InputMenu) {
    int Index = _MenuActor->GetSubmenuNum() - 1;

    _MenuActor->GetSubmenu(Index)->RemoveFrom(1);
    FindOnlineGames();
    _MenuActor->SetSubmenuByIndex(Index);
    _MenuActor->SetInputMenuLoading(Index, 0, true, "SEARCHING...");
}

void UNWGameInstance::OnButtonJoinGame(UInputMenu* InputMenu) {
    for (int32 i = 0; i < _SessionSearch->SearchResults.Num(); i++) {
        if (_SessionSearch->SearchResults[i].Session.NumOpenPublicConnections > 0) {
            if (_SessionSearch->SearchResults[i].Session.OwningUserName == InputMenu->GetFName().ToString()) {
                JoinOnlineGame(i);
                break;
            }
        }
    }
}

void UNWGameInstance::OnButtonSwitchComfortMode(UInputMenu* InputMenu) {
    _MenuOptions.bComfortMode = !_MenuOptions.bComfortMode;
    FString NewText = _MenuOptions.bComfortMode ? "COMFORT ON" : "COMFORT OFF";
    InputMenu->_TextRender->SetText(FText::FromString(NewText));
}

void UNWGameInstance::OnButtonBackToMenu(UInputMenu* InputMenu) {
    DestroySession();
}

void UNWGameInstance::OnButtonGraphics(UInputMenu* InputMenu) {
    _MenuActor->SetSubmenuByIndex(2);
}

void UNWGameInstance::OnRes1920(UInputMenu* InputMenu) {
    ChangeResolution("1920x1080f");
}

void UNWGameInstance::OnRes1600(UInputMenu* InputMenu) {
    ChangeResolution("1600x900f");
}

void UNWGameInstance::OnRes1280(UInputMenu* InputMenu) {
    ChangeResolution("1280x720f");
}

void UNWGameInstance::OnRes800(UInputMenu* InputMenu) {
    ChangeResolution("800x600f");
}

void UNWGameInstance::ChangeResolution(FString Resolution) {
    FString Command = "r.SetRes " + Resolution;
    GetFirstLocalPlayerController()->ConsoleCommand(Command);
}