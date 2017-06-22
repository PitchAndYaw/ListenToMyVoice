// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Projectile.h"

#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class LISTENTOMYVOICE_API AEnemyCharacter : public ACharacter {
    GENERATED_BODY()

public:
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Settings")
    float _SightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float _LoseSightRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float _VisionAngleDegrees;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float _HearingRange;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    class UBehaviorTree* _BehaviourTree;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI Settings")
	TSubclassOf<class UAIPerceptionComponent> AIPerceptionComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    int _Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool _IsDamaged;
	UPROPERTY(Category = DestructibleMesh, VisibleAnywhere, BlueprintReadOnly)
	class UDestructibleComponent* _DestructibleMesh;
	UPROPERTY(Category = "Diary", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _PlayerPointerComp;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, class AActor* DamageCauser) override;

    AEnemyCharacter(const FObjectInitializer& OI);

    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _BreathAudioComp;

protected:
    TAssetPtr<class UFMODEvent> _DieEvent;
    TAssetPtr<class UFMODEvent> _HurtEvent;

    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _StepsAudioComp;

    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void SetDamaged(bool Damaged);
	UFUNCTION(BlueprintCallable, Category = "Damage")
	bool GetDamaged();

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void Die();
};
