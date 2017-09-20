// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"

#include "Projectile.h"
#include "Gun.h"
#include "FMODAudioComponent.h"


// Sets default values for this component's properties
UGun::UGun(){
	municion = 250;
	RayParameter = 250;
	_ProjectileClass = AProjectile::StaticClass();
    _AreDelegatesBinded = false;
}

/*Interfaces*/
void UGun::UseItemPressed_Implementation() {
	_PlayerCharacter = Cast<APlayerCharacter>(GetOwner()->GetAttachParentActor());
    UFMODAudioComponent* AudioComp = Cast<UFMODAudioComponent>(GetOwner()->GetComponentByClass(
        UFMODAudioComponent::StaticClass()));

	if (GetAmmo() > 0) { 
		AddAmmo(-1);
		OnFire();
	}
    else if(AudioComp) AudioComp->SetParameter("bullet", 1);

    if (AudioComp) AudioComp->Play();

	ULibraryUtils::Log(FString::Printf(TEXT("Ammo : %i"), GetAmmo()), 0, 60);
}

void UGun::UseItemReleased_Implementation() {

}

void UGun::SetAmmo(uint8 AmountToSet) { municion = AmountToSet; }

void UGun::AddAmmo(uint8 AmountToAdd) { municion += AmountToAdd; }

uint8 UGun::GetAmmo() { return municion; }

void UGun::OnFire() {	
	if (_ProjectileClass != NULL) {
		UWorld* const World = GetOwner()->GetWorld();
		if (World != NULL) {
			UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			FTransform SocketTransform;
			FTransform CameraSocketTransform;
			FVector CameraVector;

			if (mesh->DoesSocketExist(TEXT("Forward"))) {
				_Socket = mesh->GetSocketByName(TEXT("Forward"));
				_Socket->GetSocketTransform(SocketTransform, mesh);
			}

			if (_PlayerCharacter && _PlayerCharacter->GetMesh()->DoesSocketExist(TEXT("ShootCamera"))) {
				_CameraSocket = _PlayerCharacter->GetMesh()->GetSocketByName(TEXT("ShootCamera"));
				CameraSocketTransform = _CameraSocket->GetSocketTransform(_PlayerCharacter->GetMesh());
			}
			
			if (_Socket) _GunEvent.Broadcast(_ProjectileClass, SocketTransform);
		}
	}
	/*Make noise*/
	if(_PlayerCharacter)
		_PlayerCharacter->MakeNoise(1.0f, GetOwner()->Instigator, GetOwner()->GetActorLocation());
}

/*********************************************** DELEGATES ***************************************/
FDelegateHandle UGun::AddOnGunDelegate(FGunDelegate& GunDelegate) {
    _AreDelegatesBinded = true;
    return _GunEvent.Add(GunDelegate);
}

void UGun::ClearOnGunDelegate(FDelegateHandle DelegateHandle) {
    _AreDelegatesBinded = false;
    _GunEvent.Remove(DelegateHandle);
}