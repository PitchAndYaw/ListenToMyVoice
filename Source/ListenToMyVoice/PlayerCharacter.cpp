// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "PlayerCharacter.h"

#include "ItfUsable.h"
#include "ItfUsableItem.h"
#include "GrabItem.h"
#include "FMODAudioComponent.h"
#include "GameModePlay.h"
#include "Walkie.h"
#include "MenuInteraction.h"


APlayerCharacter::APlayerCharacter(const FObjectInitializer& OI) :Super(OI) {
    bReplicates = true;
    bReplicateMovement = true;

    _baseTurnRate = 45.f;
    _baseLookUpRate = 45.f;
    _ItemLeft = nullptr;
    _ItemRight = nullptr;

    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    _PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
    _MenuInteractionComp = CreateDefaultSubobject<UMenuInteraction>(TEXT("Menu Interaction"));
    _MenuInteractionComp->_RayParameter = 100000;
    _MenuInteractionComp->AttachToComponent(_PlayerCamera, FAttachmentTransformRules::KeepRelativeTransform);

    _StepsAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio"));
	_BreathAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio_Breathing"));
	_PlayerPointerComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Player Pointer"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh_Plane(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (StaticMesh_Plane.Object) {
		_PlayerPointerComp->SetStaticMesh(StaticMesh_Plane.Object);
	}
	_PlayerPointerComp->SetRelativeLocation({ 0,0,300 });
	_PlayerPointerComp->bOwnerNoSee = true;

	_PlayerCamera->PostProcessBlendWeight = 0;
	_DamageDisappearVelocity = 0.3;

	OnActorHit.AddDynamic(this, &APlayerCharacter::OnHit);
    _Health = 1;
	_Damaged = false;
}

void APlayerCharacter::Tick(float DeltaSeconds) {
    Super::Tick(DeltaSeconds);
	if (_Damaged) {
		_PlayerCamera->PostProcessBlendWeight = FMath::FInterpTo(_PlayerCamera->PostProcessBlendWeight, 0.0, DeltaSeconds, _DamageDisappearVelocity);
		if (_PlayerCamera->PostProcessBlendWeight == 0) {
			_Damaged = !_Damaged;
		}
	}
}

void APlayerCharacter::AfterPossessed(bool SetInventory) {
    ToggleMenuInteraction(false);
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInput) {
    check(PlayerInput);

    /* MOVEMENT */
	PlayerInput->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
    PlayerInput->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
}

/**********************************PHYSIC MATERIALS***********************************************/
void APlayerCharacter::CheckFloorMaterial() {
	_FootSocket = GetMesh()->GetSocketByName("Foot");
	bool bRaycastHit = false;
	FCollisionQueryParams CollisionInfo;
	CollisionInfo.bReturnPhysicalMaterial = true;
	FVector StartFootRaycast;
	FVector EndFootRaycast;

	if (_FootSocket) {
		StartFootRaycast = _FootSocket->GetSocketLocation(GetMesh());
		EndFootRaycast = StartFootRaycast - (0, 0, 150);
	}

	bRaycastHit = GetWorld()->LineTraceSingleByChannel(_FootHitResult, StartFootRaycast, EndFootRaycast, ECC_Visibility, CollisionInfo);
	if (_StepsAudioComp) {
		if (_FootHitResult.PhysMaterial.IsValid() && _FootHitResult.PhysMaterial->GetName().Equals("Earth")) {
			_StepsAudioComp->SetParameter("Humedad", 0.0f);
		}
		else {
			_StepsAudioComp->SetParameter("Humedad", 1.0f);
		}
	}
}

/****************************************** ACTION MAPPINGS **************************************/
/*********** MOVEMENT ***********/
void APlayerCharacter::MoveForward(float Value) {
    if (Value != 0.0f) {
        AddMovementInput(GetActorForwardVector(), Value);
		CheckFloorMaterial();
    }
}

void APlayerCharacter::MoveRight(float Value) {
    if (Value != 0.0f) {
        AddMovementInput(GetActorRightVector(), Value);
		CheckFloorMaterial();
    }
}

void APlayerCharacter::TurnAtRate(float Rate) {
    AddControllerYawInput(Rate * _baseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate) {
    AddControllerPitchInput(Rate * _baseLookUpRate * GetWorld()->GetDeltaSeconds());
}

/************** USE *************/
bool APlayerCharacter::SERVER_UsePressed_Validate(UActorComponent* component) { return true; }
void APlayerCharacter::SERVER_UsePressed_Implementation(UActorComponent* component) {
    MULTI_UsePressed(component);
}
void APlayerCharacter::MULTI_UsePressed_Implementation(UActorComponent* component) {
    IItfUsable* itfObject = Cast<IItfUsable>(component);
    if (itfObject) {
        itfObject->Execute_UsePressed(component);
        _LastUsedPressed = component->GetOwner();
    }
}

void APlayerCharacter::UseReleased() {}
bool APlayerCharacter::SERVER_UseReleased_Validate(UActorComponent* component) { return true; }
void APlayerCharacter::SERVER_UseReleased_Implementation(UActorComponent* component) {
    MULTI_UseReleased(component);
}
void APlayerCharacter::MULTI_UseReleased_Implementation(UActorComponent* component) {
    IItfUsable* itfObject = Cast<IItfUsable>(component);
    if (itfObject) {
        itfObject->Execute_UseReleased(component);
        _LastUsedReleased = component->GetOwner();
    }
}

/******** USE ITEM LEFT *********/
void APlayerCharacter::UseLeftPressed(bool IsMenuHidden) {}

void APlayerCharacter::UseLeftReleased(bool IsMenuHidden) {}

/******* USE ITEM RIGHT *********/
void APlayerCharacter::UseRightPressed(bool IsMenuHidden) {}

void APlayerCharacter::UseRightReleased(bool IsMenuHidden) {}

/********** TAKE RIGHT HAND ***********/
bool APlayerCharacter::SERVER_TakeRight_Validate(AActor* Actor) { return true; }
void APlayerCharacter::SERVER_TakeRight_Implementation(AActor* Actor) {
    MULTI_TakeRight(Actor);
}
void APlayerCharacter::MULTI_TakeRight_Implementation(AActor* Actor) {
    UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(
                                                            UStaticMeshComponent::StaticClass()));
    UHandPickItem* HandPickComp = Cast<UHandPickItem>(Actor->FindComponentByClass(
                                                            UHandPickItem::StaticClass()));
    if (ItemMesh && HandPickComp) {
        ItemMesh->SetMobility(EComponentMobility::Movable);
        ItemMesh->SetSimulatePhysics(false);
        //ItemMesh->AttachToComponent(GetMesh(),
        //                        FAttachmentTransformRules::KeepRelativeTransform,
        //                       TEXT("GripPoint_R"));

        //ItemMesh->RelativeLocation = HandPickComp->_locationAttach_R;
        //ItemMesh->RelativeRotation = HandPickComp->_rotationAttach_R;

        Actor->SetActorEnableCollision(false);
        _ItemRight = Actor;
    }
}

/********** TAKE LEFT HAND ***********/
bool APlayerCharacter::SERVER_TakeLeft_Validate(AActor* Actor) { return true; }
void APlayerCharacter::SERVER_TakeLeft_Implementation(AActor* Actor) {
    MULTI_TakeLeft(Actor);
}
void APlayerCharacter::MULTI_TakeLeft_Implementation(AActor* Actor) {
    UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(
                                                            UStaticMeshComponent::StaticClass()));
	
    UHandPickItem* HandPickComp = Cast<UHandPickItem>(Actor->FindComponentByClass(
        UHandPickItem::StaticClass()));
    if (ItemMesh && HandPickComp) {
		const UStaticMeshSocket* GripSocket = ItemMesh->GetSocketByName(TEXT("Grip"));
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(TEXT("GripPoint_L"));
        ItemMesh->SetMobility(EComponentMobility::Movable);
        ItemMesh->SetSimulatePhysics(false);
		ItemMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "itemHand_l");
		
		//ItemMesh->SetRelativeLocation(HandSocket->RelativeLocation);
		//ItemMesh->RelativeRotation = GripSocket->RelativeRotation;
		

        Actor->SetActorEnableCollision(false);
        _ItemLeft = Actor;
    }
}

/********** DROP HAND ***********/
bool APlayerCharacter::SERVER_Drop_Validate(AActor* ItemActor, int Hand) { return true; }
void APlayerCharacter::SERVER_Drop_Implementation(AActor* ItemActor, int Hand) {
    CLIENT_ClearRadioDelegates(ItemActor);
    MULTI_Drop(ItemActor, Hand);
}
void APlayerCharacter::MULTI_Drop_Implementation(AActor* ItemActor, int Hand) {
    UStaticMeshComponent* ItemMesh = Cast<UStaticMeshComponent>(ItemActor->GetComponentByClass(
                                                            UStaticMeshComponent::StaticClass()));
    if (ItemMesh) {
        ItemMesh->SetMobility(EComponentMobility::Movable);
        ItemMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
        ItemMesh->SetSimulatePhysics(true);

        ItemActor->SetActorEnableCollision(true);

        if (Hand == 1) _ItemLeft = nullptr;
        else if (Hand == 2) _ItemRight = nullptr;
    }
}

/****************************************** AUXILIAR FUNCTIONS * **********************************/
void APlayerCharacter::ToggleMenuInteraction(bool Activate) {
    _MenuInteractionComp->SetActive(Activate);
    _MenuInteractionComp->SetHiddenInGame(!Activate, true);
    _MenuInteractionComp->SetComponentTickEnabled(Activate);
    _MenuInteractionComp->SetVisibility(Activate, true);
}

AActor* APlayerCharacter::GetWalkieActor() { 
    return _WalkieActor ? _WalkieActor : nullptr; 
}

bool APlayerCharacter::IsWalkieInHand() {
    return _WalkieActor ? _WalkieActor == _ItemLeft || _WalkieActor == _ItemRight : false;
}

UFMODEvent* APlayerCharacter::GetWalkieEvent() {
    if (_WalkieActor) {
        UWalkie* Walkie = Cast<UWalkie>(_WalkieActor->GetComponentByClass(UWalkie::StaticClass()));
        return Walkie->_NoiseEvent;
    }
    return nullptr;
}

/******* Radio Delegate *******/
void APlayerCharacter::CLIENT_AddRadioDelegates_Implementation(AActor* Actor) {
    if (Actor) {
        UWalkie* Walkie = Cast<UWalkie>(Actor->GetComponentByClass(UWalkie::StaticClass()));
        if (Walkie && !Walkie->_AreDelegatesBinded) {
            _OnRadioPressedDelegateHandle = Walkie->AddOnRadioDelegate(_OnRadioPressedDelegate, true);
            _OnRadioReleasedDelegateHandle = Walkie->AddOnRadioDelegate(_OnRadioReleasedDelegate, false);

            _WalkieActor = Actor;
            
            const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
            FString myRole = EnumPtr->GetEnumName((int32)Role);
            ULibraryUtils::Log(FString::Printf(TEXT("AddRadioDelegates : %s"), *myRole), 0, 60);
        }
    }
}

void APlayerCharacter::CLIENT_ClearRadioDelegates_Implementation(AActor* Actor) {
    if (Actor) {
        UWalkie* Walkie = Cast<UWalkie>(Actor->GetComponentByClass(UWalkie::StaticClass()));
        if (Walkie && Walkie->_AreDelegatesBinded) {
            Walkie->ClearOnRadioDelegate(_OnRadioPressedDelegateHandle, true);
            Walkie->ClearOnRadioDelegate(_OnRadioReleasedDelegateHandle, false);

            _WalkieActor = Actor;

            const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true);
            FString myRole = EnumPtr->GetEnumName((int32)Role);
            ULibraryUtils::Log(FString::Printf(TEXT("ClearRadioDelegates : %s"), *myRole), 0, 60);
        }
    }
}

/***********RECEIVE HIT AND DAMAGE*************/
void APlayerCharacter::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor) {
		if (OtherActor->IsA(AProjectile::StaticClass())) {
			// Create a damage event  
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);
			const float DamageAmount = 1.0f;
			TakeDamage(DamageAmount, DamageEvent, GetController(), OtherActor);
		}
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                   class AController* EventInstigator, class AActor* DamageCauser) {	
    _Health -= DamageAmount;	
	/*Fade to red when take damage*/
	_PlayerCamera->PostProcessBlendWeight = 1;
	_PlayerCamera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	_PlayerCamera->PostProcessSettings.SceneFringeIntensity = 5.0f;
	_Damaged = true;

	if (_Health <= 0) {
		AGameModePlay* GameMode = Cast<AGameModePlay>(GetWorld()->GetAuthGameMode());
        if (GameMode) {
			GameMode->SERVER_PlayerDead(GetController());
            MULTI_CharacterDead();
        }
    }
    return _Health;	
}

void APlayerCharacter::MULTI_CharacterDead_Implementation() {
	Cast<UPrimitiveComponent>(GetRootComponent())->SetCollisionProfileName(FName("Ragdoll"));
	SetActorEnableCollision(true);
	GetMesh()->SetSimulatePhysics(true);
}

