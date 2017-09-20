// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ItfUsableItem.h"
#include "PlayerCharacter.h"

#include "Components/ActorComponent.h"
#include "Gun.generated.h"

DECLARE_EVENT_TwoParams(UGun, FGunEvent, TSubclassOf<AActor>, const FTransform);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LISTENTOMYVOICE_API UGun : public UActorComponent, public IItfUsableItem {
	GENERATED_BODY()

public:
    bool _AreDelegatesBinded;

    FDelegateHandle AddOnGunDelegate(FGunDelegate& RadioDelegate);
    void ClearOnGunDelegate(FDelegateHandle DelegateHandle);

	UGun();
	void OnFire();
	float RayParameter;	

protected:
	//Only needed to shoot projectile from camera (and make noise?)
	class APlayerCharacter* _PlayerCharacter;

public:	
	void SetAmmo(uint8 AmountToSet);
	void AddAmmo(uint8 AmountToAdd);
	uint8 GetAmmo();

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class AProjectile> _ProjectileClass;

	const UStaticMeshSocket* _Socket;
	const USkeletalMeshSocket* _CameraSocket;

	/*Inerfaces*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
	void UseItemPressed();
	virtual void UseItemPressed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Use Item")
	void UseItemReleased();
	virtual void UseItemReleased_Implementation() override;
		
private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Gun Component", meta = (AllowPrivateAccess = "true"))
	uint8 municion;

    FGunEvent _GunEvent;
};
