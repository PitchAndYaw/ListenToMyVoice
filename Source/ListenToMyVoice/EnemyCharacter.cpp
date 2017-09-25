// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "EnemyCharacter.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

#include "EnemyController.h"


void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyCharacter, _IsPossessed);
}

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& OI) : Super(OI) {
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bReplicateMovement = true;

    _IsPossessed = false;

    _StepsAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio"));
    _StepsAudioComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Foot"));
    _StepsAudioComp->Event = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Personaje/pasos.pasos")));
    _StepsAudioComp->ComponentTags.Add("step");
    _StepsAudioComp->bAutoActivate = false;

    _BreathAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio_Breathing"));
    _BreathAudioComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _BreathAudioComp->Event = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Personaje/Beast.Beast")));
    _BreathAudioComp->bAutoActivate = false;

    _HurtAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio_Hurt"));
    _HurtAudioComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _HurtAudioComp->Event = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Personaje/HurtEnemy.HurtEnemy")));
    _HurtAudioComp->bAutoActivate = false;

	_PlayerPointerComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Player Pointer"));
    _PlayerPointerComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _PlayerPointerComp->SetRelativeLocation({ 0, 0, 500 });
    _PlayerPointerComp->SetRelativeRotation({ 90, 0, -90 });
    _PlayerPointerComp->bOwnerNoSee = true;

    _DieEvent = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Scene/EnemyDead.EnemyDead")));

    AIControllerClass = AEnemyController::StaticClass();

	_DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleMesh"));
	_DestructibleMesh->SetHiddenInGame(true);
	_DestructibleMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	_DestructibleMesh->SetRelativeLocation(GetMesh()->GetComponentLocation());

    AutoPossessAI = EAutoPossessAI::Disabled;
    _SightRadius = 500.0f;
    _LoseSightRadius = 1000.0f;
    _VisionAngleDegrees = 90.0f;
	_HearingRange = 1500.0f;
    _Damage = 1;
	_IsDamaged = false;
    _IsDead = false;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
}

bool AEnemyCharacter::SERVER_SetIsPossessed_Validate(bool IsPossessed) { return true; }
void AEnemyCharacter::SERVER_SetIsPossessed_Implementation(bool IsPossessed) {
    _IsPossessed = IsPossessed;
    OnRep_SetIsPossessed();
}

void AEnemyCharacter::OnRep_SetIsPossessed() {
    ULibraryUtils::Log("OnRep_SetIsPossessed");

    if (_IsPossessed && Role != ROLE_Authority) {
        _BreathAudioComp->Play();
        ULibraryUtils::Log("_BreathAudioComp->Play()");
    }
}


bool AEnemyCharacter::SERVER_TakeDamage_Validate(int DamageAmount) { return true; }
void AEnemyCharacter::SERVER_TakeDamage_Implementation(int DamageAmount) {
    if (!_IsDead) {
        ULibraryUtils::Log(FString::Printf(TEXT("AEnemyCharacter::TakeDamage")), 0, 60);
        /*The enemy doesn't receive damage*/
        SetDamaged(true);
    }
    MULTI_TakeDamage(DamageAmount);
}

void AEnemyCharacter::MULTI_TakeDamage_Implementation(int DamageAmount) {
    if (_HurtAudioComp) _HurtAudioComp->Play();
}

void AEnemyCharacter::SetDamaged(bool Damaged) {
    ULibraryUtils::Log("AEnemyCharacter::SetDamaged");
	_IsDamaged = Damaged;
}

bool AEnemyCharacter::GetDamaged() {
	return _IsDamaged;
}

void AEnemyCharacter::Die() {
	if (_DestructibleMesh) {
		GetMesh()->SetHiddenInGame(true);
		//_DestructibleMesh->SetSimulatePhysics(true);
		_DestructibleMesh->SetEnableGravity(true);
		_DestructibleMesh->SetHiddenInGame(false);
		_DestructibleMesh->ApplyRadiusDamage(100.0f, GetActorLocation(), 100.0f, 0.0f, false);

        _StepsAudioComp->Event = _DieEvent;
        _StepsAudioComp->Play();

        _BreathAudioComp->Stop();

        _IsDead = true;

        SetActorTickEnabled(false);

		if(GetController())
			GetController()->UnPossess();
	}
}