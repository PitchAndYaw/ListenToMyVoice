// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "Menu3D.h"

#include "MenuPanel.h"
#include "InputMenu.h"


AMenu3D::AMenu3D(const class FObjectInitializer& OI) : Super(OI) {
    PrimaryActorTick.bCanEverTick = true;

    /* SOUNDS */
    _FirstTime = true;
    _AudioOpenCloseEvent = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/UI/Ok2.Ok2")));
    _AudioSessionOkEvent = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/UI/Tic.Tic")));
    _AudioSessionKoEvent = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/UI/Cancel2.Cancel2")));

    _AudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("_AudioComp"));
    _AudioComp->bAutoActivate = false;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root Component")));

    /*** DECORATORS ***/
    _TopDecorator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("_TopDecorator"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(
        TEXT("StaticMesh'/Game/Art/Common/Menu/Meshes/menu2_parte_superior.menu2_parte_superior'"));
    _TopDecorator->SetStaticMesh(Finder.Object);
    _TopDecorator->LightingChannels.bChannel1 = true;

    _BottomDecorator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("_BottomDecorator"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Finder2(
        TEXT("StaticMesh'/Game/Art/Common/Menu/Meshes/menu2_parte_abajo.menu2_parte_abajo'"));
    _BottomDecorator->SetStaticMesh(Finder2.Object);
    _BottomDecorator->LightingChannels.bChannel1 = true;

    _MiddleDecorator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("_MiddleDecorator"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Finder3(
        TEXT("StaticMesh'/Game/Art/Common/Menu/Meshes/menu2_parte_intermedia.menu2_parte_intermedia'"));
    _MiddleDecorator->SetStaticMesh(Finder3.Object);
    _MiddleDecorator->LightingChannels.bChannel1 = true;

    _BackSubmenu = CreateDefaultSubobject<UInputMenu>(TEXT("BACK"));
    _BackSubmenu->_InputMenuReleasedDelegate.BindUObject(this, &AMenu3D::OnButtonBack);
    _BackSubmenu->AddOnInputMenuDelegate();

    _BackSubmenu->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _TopDecorator->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _BottomDecorator->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _MiddleDecorator->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

    _IsMenuHidden = true;
    _Submenus = {};
    _Breadcrumb = {};

    FVector Origin;
    FVector BoxExtent;
    float SphereRadius;

    UKismetSystemLibrary::GetComponentBounds(_BackSubmenu, Origin, BoxExtent, SphereRadius);
    _BackMenuSize = 2 * BoxExtent.Z;
}

void AMenu3D::BeginPlay() {
    Super::BeginPlay();

    _TopDecorator->SetCollisionProfileName("IgnoreOnlyPawn");
    _MiddleDecorator->SetCollisionProfileName("IgnoreOnlyPawn");
    _BottomDecorator->SetCollisionProfileName("IgnoreOnlyPawn");
    _BackSubmenu->SetCollisionProfileName("IgnoreOnlyPawn");
}

void AMenu3D::AddSubmenu(UMenuPanel* Submenu) {
    Submenu->RegisterComponent();
    Submenu->AttachToComponent(_TopDecorator, FAttachmentTransformRules::KeepRelativeTransform);
    _Submenus.Add(Submenu);
}

void AMenu3D::ToogleMenu(FVector Location, FRotator Rotation) {
    if (_FirstTime) {
        _AudioComp->Event = _AudioOpenCloseEvent;
        _AudioComp->Play();
        _FirstTime = false;
    }
    if (_IsMenuHidden) {
        SetSubmenuByIndex(0);
        
        SetActorHiddenInGame(false);
        SetActorTickEnabled(true);

        SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
    }
    else {
        SetActorHiddenInGame(true);
        SetActorTickEnabled(false);
        
        SetSubmenuByIndex(-1);
    }
    _IsMenuHidden = !_IsMenuHidden;
}

void AMenu3D::SetSubmenuByIndex(const int& Index) {
    for (int i = 0; i < _Submenus.Num(); i++) {
        if (i == Index) {
            _Submenus[i]->EnablePanel(true);

            PlaceDecorators(!(Index == 0), _Submenus[i]->_PanelHeight);
            _Breadcrumb.Add(Index);
        }
        else {
            _Submenus[i]->EnablePanel(false);
        }
    }

    if (Index < 0) _Breadcrumb.Reset();
}

void AMenu3D::SetInputMenuLoading(int IndexPanel, int IndexInputMenu, bool IsLoading,
                                  FString Text) {
    if (IndexPanel < _Submenus.Num()) {
        UInputMenu* InputMenu = _Submenus[IndexPanel]->GetInputMenuAt(IndexInputMenu);
        if(InputMenu) InputMenu->SetLoading(IsLoading, Text);
    }
}

/*********************************** BINDINGS ****************************************************/
void AMenu3D::OnButtonBack(UInputMenu* InputMenu) {
    _Breadcrumb.RemoveAt(_Breadcrumb.Num() - 1);
    int Aux = _Breadcrumb.Top();
    _Breadcrumb.RemoveAt(_Breadcrumb.Num() - 1);

    SetSubmenuByIndex(Aux);
}

/*********************************** AUXILIAR ****************************************************/
void AMenu3D::PlaceDecorators(bool PlaceBackButton, float PanelHeight) {
    FVector Location = FVector(0, 0, -PanelHeight);
    if (PlaceBackButton) {
        _BackSubmenu->Enable(true);
        _BackSubmenu->Init(FVector(0, 0, -PanelHeight));

        Location.Z -= _BackMenuSize + 10;
    }
    else {
        _BackSubmenu->Enable(false);
    }

    _BottomDecorator->SetRelativeLocation(Location);
    Location.Z = -(PanelHeight + (PlaceBackButton ? _BackMenuSize : 0)) / 2;
    _MiddleDecorator->SetRelativeLocation(Location);
}

int AMenu3D::GetSubmenuNum() {
    return _Submenus.Num();
}

void AMenu3D::PlayEndFindSessions(bool Ok) {
    _AudioComp->Event = Ok ? _AudioSessionOkEvent : _AudioSessionKoEvent;
    _AudioComp->Play();
}