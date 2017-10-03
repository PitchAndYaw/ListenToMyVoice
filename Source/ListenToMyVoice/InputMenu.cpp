// Fill  out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "InputMenu.h"

#include "FMODAudioComponent.h"


UInputMenu::UInputMenu(const FObjectInitializer& OI) : Super(OI) {
    PrimaryComponentTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(
        TEXT("StaticMesh'/Game/Art/Common/Menu/Meshes/menu2_cajaprueba.menu2_cajaprueba'"));
    SetStaticMesh(Finder.Object);
    LightingChannels.bChannel1 = true;

    _TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("_TextRender"));
    
    static ConstructorHelpers::FObjectFinder<UMaterial> Finder2(
        TEXT("Material'/Game/Art/Common/Fonts/TextLTMVMaterial.TextLTMVMaterial'"));
    _Material = Finder2.Object;
    static ConstructorHelpers::FObjectFinder<UMaterial> Finder3(
        TEXT("Material'/Game/Art/Common/Fonts/TextLTMVMaterialHover.TextLTMVMaterialHover'"));
    _MaterialHover = Finder3.Object;
    _TextRender->SetMaterial(0, _Material);

    FStringAssetReference AssetRef(TEXT("/Game/Art/Common/Fonts/FontIcarus.FontIcarus"));
    _TextRender->SetFont(TAssetPtr<UFont>(AssetRef).Get());

    _TextRender->SetWorldSize(12);
    _TextRender->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    _TextRender->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

    _AudioHoverEvent = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/UI/Tic.Tic")));
    _AudioClickEvent = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/UI/Ok.Ok")));
    
    _AudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("_AudioComp"));
    _AudioComp->bAutoActivate = false;

    _NextPoint = FVector();
    bool _IsFlee = false;
    _IsLoading = false;
}

void UInputMenu::BeginPlay() {
    _TextRender->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform,
                                   FName("SocketText"));
    _TextRender->RegisterComponent();

    _AudioComp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
    _AudioComp->RegisterComponent();

    SetCollisionProfileName("IgnoreOnlyPawn");
}

void UInputMenu::Init(const FVector MenuPanelLocation) {
    _InitialLocation = MenuPanelLocation;
    RelativeLocation = _InitialLocation;

    UpdateNextPoint();
    bool _IsFlee = true;
}

void UInputMenu::TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (FVector::Dist(RelativeLocation, _NextPoint) <= 3) {
        if (_IsFlee) _NextPoint = _InitialLocation;
        else UpdateNextPoint();
        _IsFlee = !_IsFlee;
    }
    AddRelativeLocation((_NextPoint - RelativeLocation).GetSafeNormal() * 1.5 * DeltaTime);

    /* LOADING */
    if (_IsLoading) AddRelativeRotation(FRotator(0, 150 * DeltaTime, 0));
}

void UInputMenu::UpdateNextPoint() {
    _NextPoint.X = _InitialLocation.X + FMath::FRandRange(-5, 5);
    _NextPoint.Y = _InitialLocation.Y + FMath::FRandRange(-5, 5);
    _NextPoint.Z = _InitialLocation.Z + FMath::FRandRange(-5, 5);
}

void UInputMenu::PressEvents() {
    if (!_IsLoading) {
        _TextRender->SetMaterial(0, _Material);
        _InputMenuPressedEvent.Broadcast(this);
    }
}

void UInputMenu::ReleaseEvents() {
    if (!_IsLoading) {
        _AudioComp->Event = _AudioClickEvent;
        _TextRender->SetMaterial(0, _MaterialHover);
        _AudioComp->Play();
        _InputMenuReleasedEvent.Broadcast(this);
    }
}

void UInputMenu::HoverInteraction() {
    if (_TextRender) {
        _AudioComp->Event = _AudioHoverEvent;
        _TextRender->SetMaterial(0, _MaterialHover);
        _AudioComp->Play();
    }
}

void UInputMenu::EndhoverInteraction() {
    if (_TextRender) {
        _TextRender->SetMaterial(0, _Material);
    }
}

void UInputMenu::Enable(bool Enable) {
    SetActive(Enable);
    SetHiddenInGame(!Enable, true);
    SetComponentTickEnabled(Enable);
    SetVisibility(Enable, true);
    if (Enable) SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    else SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UInputMenu::SetLoading(bool IsLoading, FString Text) {
    _IsLoading = IsLoading;
    if (_IsLoading) {
        _PrevText = _TextRender->Text;
        _TextRender->SetText(FText::FromString(Text));
    }
    else {
        _TextRender->SetText(Text.IsEmpty() ? _PrevText : FText::FromString(Text));
        SetRelativeRotation(FRotator(0, 0, 0));
    }
}

/*********************************************** DELEGATES ***************************************/
void UInputMenu::AddOnInputMenuDelegate() {
    if(_InputMenuPressedDelegate.IsBound())
        _OnInputMenuPressedDelegateHandle = _InputMenuPressedEvent.Add(_InputMenuPressedDelegate);
    
    if (_InputMenuReleasedDelegate.IsBound())
        _OnInputMenuReleasedDelegateHandle = _InputMenuReleasedEvent.Add(_InputMenuReleasedDelegate);
}

void UInputMenu::ClearOnInputMenuDelegate() {
    _InputMenuPressedEvent.Remove(_OnInputMenuPressedDelegateHandle);
    _InputMenuReleasedEvent.Remove(_OnInputMenuReleasedDelegateHandle);
}