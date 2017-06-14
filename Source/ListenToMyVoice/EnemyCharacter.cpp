// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "EnemyCharacter.h"

#include "EnemyController.h"

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& OI) : Super(OI) {
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bReplicateMovement = true;

    AIControllerClass = AEnemyController::StaticClass();
	OnActorHit.AddDynamic(this, &AEnemyCharacter::OnHit);

	_DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleMesh"));
	_DestructibleMesh->SetHiddenInGame(true);
	//_DestructibleMesh->SetNotifyRigidBodyCollision(true);
	_DestructibleMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	_DestructibleMesh->SetRelativeLocation(GetMesh()->GetComponentLocation());

    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
    AutoPossessAI = EAutoPossessAI::Disabled;
    _SightRadius = 500.0f;
    _LoseSightRadius = 1000.0f;
    _VisionAngleDegrees = 90.0f;
	_HearingRange = 1500.0f;
    _Damage = 1;
	_IsDamaged = false;

	GetCharacterMovement()->MaxWalkSpeed = 170.0f;
}

void AEnemyCharacter::BeginPlay() {
    Super::BeginPlay();
}

void AEnemyCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void AEnemyCharacter::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor) {
		if (OtherActor->IsA(AProjectile::StaticClass())) {
			// Create a damage event  
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);

			const float DamageAmount = 0.0f;
			TakeDamage(DamageAmount, DamageEvent, GetController(), OtherActor);
		}
	}
}

float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
				 class AController* EventInstigator, class AActor* DamageCauser) {
	ULibraryUtils::Log(FString::Printf(TEXT("Me han dado")), 0, 60);
	/*The enemy doesn't receive damage*/
	SetDamaged(true);
	return 0.0f;
}

void AEnemyCharacter::SetDamaged(bool Damaged) {
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
		if(GetController())
			GetController()->UnPossess();
	}
}