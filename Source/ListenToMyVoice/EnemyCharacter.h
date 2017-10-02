#pragma once


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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    int _Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool _IsDamaged;
	UPROPERTY(Category = DestructibleMesh, VisibleAnywhere, BlueprintReadOnly)
	class UDestructibleComponent* _DestructibleMesh;
	UPROPERTY(Category = "Diary", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* _PlayerPointerComp;

    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _BreathAudioComp;
    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _ActionAudioComp;

    UPROPERTY(ReplicatedUsing = OnRep_SetIsPossessed)
    bool _IsPossessed;
    UFUNCTION(reliable, server, WithValidation)
    void SERVER_SetIsPossessed(bool IsPossessed);
    UFUNCTION()
    virtual void OnRep_SetIsPossessed();

    AEnemyCharacter(const FObjectInitializer& OI);
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_TakeDamage(int DamageAmount);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_TakeDamage(int DamageAmount);

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_MakeDamage(ACharacter* Target, int DamageAmount);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_MakeDamage(int DamageAmount);

protected:
    bool _IsDead;

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
