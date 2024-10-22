#pragma once


#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class LISTENTOMYVOICE_API APlayerCharacter : public ACharacter {
    GENERATED_BODY()

public:
    AActor* _ItemLeft;
    AActor* _ItemRight;

    AActor* _LastItemUsedPressedLeft;
    AActor* _LastItemUsedReleasedLeft;
    AActor* _LastItemUsedPressedRight;
    AActor* _LastItemUsedReleasedRight;

    AActor* _LastUsedPressed;
    AActor* _LastUsedReleased;

    bool _ItemPressedLeft;
    bool _ItemPressedRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    int _Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
	float _DamageDisappearVelocity;

	void Tick(float DeltaSeconds) override;

    /* Radio Delegate */
    FRadioDelegate _OnRadioPressedDelegate;
    FRadioDelegate _OnRadioReleasedDelegate;

    /* Gun Delegate */
    FGunDelegate _OnGunDelegate;
    void SpawnProjectile(TSubclassOf<AActor> C, const FTransform T);

    
    APlayerCharacter(const FObjectInitializer& OI);
    virtual void AfterPossessed(bool SetInventory);

    /******** USE ITEM LEFT *********/
    virtual void UseLeftPressed(bool IsMenuHidden);
    virtual void UseLeftReleased(bool IsMenuHidden);

    /******* USE ITEM RIGHT *********/
    virtual void UseRightPressed(bool IsMenuHidden);
    virtual void UseRightReleased(bool IsMenuHidden);

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_TakeDamage(int DamageAmount);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_TakeDamage(int DamageAmount);

    AActor* GetWalkieActor();
    bool IsWalkieInHand();
    class UFMODEvent* GetWalkieEvent();

    virtual void ToggleMenuInteraction(bool Activate);


    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_SpawnActor(TSubclassOf<AActor> C, const FTransform T);

protected:
    UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* _PlayerCamera;
    UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UFMODAudioComponent* _StepsAudioComp;
	UPROPERTY(Category = Audio, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UFMODAudioComponent* _BreathAudioComp;
    UPROPERTY(Category = Menu , VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UMenuInteraction* _MenuInteractionComp;
	UPROPERTY(Category = Diary, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* _PlayerPointerComp;
    UPROPERTY(Category = AI, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UAIPerceptionStimuliSourceComponent* _PerceptionStimuliSource;

    void BeginPlay() override;
    void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    /*************************************** ACTION MAPPINGS *************************************/
    /*********** MOVEMENT ***********/
    virtual void MoveForward(float Val);

    /************** USE *************/
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_UsePressed(UActorComponent* component);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_UsePressed(UActorComponent* component);

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_UseReleased(UActorComponent* component);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_UseReleased(UActorComponent* component);


    /********** TAKE ITEM ***********/
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_TakeRight(AActor* Actor);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_TakeRight(AActor* Actor);
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_TakeLeft(AActor* Actor);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_TakeLeft(AActor* Actor);

    
    /********** DROP ITEM ***********/
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_Drop(AActor* ItemActor, int Hand);
    UFUNCTION(NetMulticast, Reliable)
    void MULTI_Drop(AActor* ItemActor, int Hand);
    /*
    Hand = 0 => void
    Hand = 1 => _ItemLeft
    Hand = 2 => _ItemRight
    */

    UFUNCTION(NetMulticast, Reliable)
    void MULTI_CharacterDead();
    UFUNCTION(Client, Reliable)


    /* DELEGATES */
    void CLIENT_AddRadioDelegates(AActor* Actor);
    UFUNCTION(Client, Reliable)
    void CLIENT_ClearRadioDelegates(AActor* Actor);

    UFUNCTION(Client, Reliable)
    void CLIENT_AddGunDelegates(AActor* Actor);
    UFUNCTION(Client, Reliable)
    void CLIENT_ClearGunDelegates(AActor* Actor);

    /* Physic Materials */
    const USkeletalMeshSocket* _FootSocket;
    void CheckFloorMaterial();
    FHitResult _FootHitResult;

    void UseReleasedFocusOut();

private:
    AActor* _WalkieActor;

    /* Radio Delegate */
    FDelegateHandle _OnRadioPressedDelegateHandle;
    FDelegateHandle _OnRadioReleasedDelegateHandle;

    /* Gun Delegate */
    FDelegateHandle _OnGunDelegateHandle;

    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_ReportNoise(float Loudness = 1.0f);

public:
    FORCEINLINE UCameraComponent* APlayerCharacter::GetPlayerCamera() const { return _PlayerCamera; }
    FORCEINLINE UFMODAudioComponent* APlayerCharacter::GetStepsAudioComp() const { return _StepsAudioComp; }
	FORCEINLINE UFMODAudioComponent* APlayerCharacter::GetBreathAudioComp() const { return _BreathAudioComp; }
    FORCEINLINE UMenuInteraction* APlayerCharacter::GetMenuInteractionComp() const { return _MenuInteractionComp; }
    FORCEINLINE UAIPerceptionStimuliSourceComponent* APlayerCharacter::GetPerceptionStimuliSource() const { return _PerceptionStimuliSource; }
};