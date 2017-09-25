// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Projectile.h"

#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class LISTENTOMYVOICE_API AEnemyCharacter : public ACharacter {
    GENERATED_BODY()

public:
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
	class UPaperSpriteComponent* _PlayerPointerComp;
    
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_TakeDamage(int DamageAmount);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_TakeDamage(int DamageAmount);

    AEnemyCharacter(const FObjectInitializer& OI);

    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _BreathAudioComp;
    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _HurtAudioComp;

    UFUNCTION(NetMulticast, Reliable)
    void MULTI_AfterPossessed();

protected:
    bool _IsDead;
    TAssetPtr<class UFMODEvent> _DieEvent;

    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _StepsAudioComp;

public:
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void SetDamaged(bool Damaged);
	UFUNCTION(BlueprintCallable, Category = "Damage")
	bool GetDamaged();

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void Die();
};
