// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "EnemyCharacter.h"

#include "EnemyController.h"

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& OI) : Super(OI) {
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bReplicateMovement = true;

    _StepsAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio"));
    _StepsAudioComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Foot"));
    _StepsAudioComp->Event = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Personaje/pasos.pasos")));
    _StepsAudioComp->ComponentTags.Add("step");
    _StepsAudioComp->bAutoActivate = false;

    _BreathAudioComp = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("Audio_Breathing"));
    _BreathAudioComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    _BreathAudioComp->Event = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Personaje/Beast.Beast")));
    _BreathAudioComp->bAutoActivate = false;

	_PlayerPointerComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Player Pointer"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh_Plane(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (StaticMesh_Plane.Object) {
		_PlayerPointerComp->SetStaticMesh(StaticMesh_Plane.Object);
	}
	_PlayerPointerComp->SetRelativeLocation({ 0,0,300 });
	_PlayerPointerComp->bOwnerNoSee = true;

    _DieEvent = TAssetPtr<UFMODEvent>(FStringAssetReference(TEXT("/Game/FMOD/Events/Scene/EnemyDead.EnemyDead")));

    AIControllerClass = AEnemyController::StaticClass();
	OnActorHit.AddDynamic(this, &AEnemyCharacter::OnHit);

	_DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleMesh"));
	_DestructibleMesh->SetHiddenInGame(true);
	//_DestructibleMesh->SetNotifyRigidBodyCollision(true);
	_DestructibleMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	_DestructibleMesh->SetRelativeLocation(GetMesh()->GetComponentLocation());

    AutoPossessAI = EAutoPossessAI::Disabled;
    _SightRadius = 500.0f;
    _LoseSightRadius = 1000.0f;
    _VisionAngleDegrees = 90.0f;
	_HearingRange = 1500.0f;
    _Damage = 1;
	_IsDamaged = false;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
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

        _BreathAudioComp->Event = _DieEvent;
        _BreathAudioComp->Play();

		if(GetController())
			GetController()->UnPossess();
	}
}