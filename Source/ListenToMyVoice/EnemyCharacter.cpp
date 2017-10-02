// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "EnemyCharacter.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

#include "EnemyController.h"
#include "PlayerCharacter.h"


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
    _StepsAudioComp->SetParameter("Humedad", 0.0f);

    _BreathAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio_Breathing"));
    _BreathAudioComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _BreathAudioComp->Event = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Personaje/Beast.Beast")));
    _BreathAudioComp->bAutoActivate = false;

    _ActionAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("_ActionComp"));
    _ActionAudioComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _ActionAudioComp->Event = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Personaje/HurtEnemy.HurtEnemy")));
    _ActionAudioComp->bAutoActivate = false;
    _ActionAudioComp->SetParameter("Mode", 0.0f);

	_PlayerPointerComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Player Pointer"));
    _PlayerPointerComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _PlayerPointerComp->SetRelativeLocation({ 0, 0, 500 });
    _PlayerPointerComp->SetRelativeRotation({ 90, 0, -90 });
    _PlayerPointerComp->bOwnerNoSee = true;

    AIControllerClass = AEnemyController::StaticClass();

	_DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleMesh"));
	_DestructibleMesh->SetHiddenInGame(true);
	_DestructibleMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	_DestructibleMesh->SetRelativeLocation(GetMesh()->GetComponentLocation());

    AutoPossessAI = EAutoPossessAI::Disabled;
    _SightRadius = 500.0f;
    _LoseSightRadius = 1000.0f;
    _VisionAngleDegrees = 90.0f;
	_HearingRange = 700.0f;
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
    if (_IsPossessed && Role != ROLE_Authority) _BreathAudioComp->Play();
}

bool AEnemyCharacter::SERVER_TakeDamage_Validate(int DamageAmount) { return true; }
void AEnemyCharacter::SERVER_TakeDamage_Implementation(int DamageAmount) {
    if (!_IsDead) {
        SetDamaged(true);
        MULTI_TakeDamage(DamageAmount);
    }
}

void AEnemyCharacter::MULTI_TakeDamage_Implementation(int DamageAmount) {
    if (_ActionAudioComp) {
        _ActionAudioComp->SetParameter("Mode", 0.0f);
        _ActionAudioComp->Play();
    }
}

bool AEnemyCharacter::SERVER_MakeDamage_Validate(ACharacter* Target, int DamageAmount) { return true; }
void AEnemyCharacter::SERVER_MakeDamage_Implementation(ACharacter* Target, int DamageAmount) {
    APlayerCharacter* TargetCharacter = Cast<APlayerCharacter>(Target);
    if (!_IsDead && TargetCharacter) {
        MULTI_MakeDamage(DamageAmount);
        TargetCharacter->SERVER_TakeDamage(DamageAmount);
    }
}

void AEnemyCharacter::MULTI_MakeDamage_Implementation(int DamageAmount) {
    if (_ActionAudioComp) {
        _ActionAudioComp->SetParameter("Mode", 0.4f);
        _ActionAudioComp->Play();
    }
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

        _BreathAudioComp->Stop();
        _StepsAudioComp->Stop();

        if (_ActionAudioComp) {
            _ActionAudioComp->SetParameter("Mode", 1.0f);
            _ActionAudioComp->Play();
        }

        _IsDead = true;
        SetActorTickEnabled(false);

		if(GetController())	GetController()->UnPossess();
	}
}