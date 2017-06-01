// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"

#include "Projectile.h"
#include "Gun.h"


// Sets default values for this component's properties
UGun::UGun(){
	municion = 250;
	RayParameter = 250;
	_ProjectileClass = AProjectile::StaticClass();
}

/*Interfaces*/
void UGun::UseItemPressed_Implementation() {
	_PlayerCharacter = Cast<APlayerCharacter>(GetOwner()->GetAttachParentActor());

	if (GetAmmo() > 0) { 
		AddAmmo(-1);
		OnFire();
	}
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

			if (mesh->DoesSocketExist(TEXT("Forward"))) {
				_Socket = mesh->GetSocketByName(TEXT("Forward"));
				_Socket->GetSocketTransform(SocketTransform, mesh);
			}

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Owner = GetOwner();
			ActorSpawnParams.Instigator = GetOwner()->Instigator;

			
			//Shoot bullet from gun:
			if (_Socket) {
				AProjectile* const NewProjectile = World->SpawnActor<AProjectile>
					(_ProjectileClass, SocketTransform, ActorSpawnParams);
			}

			//Shoot bullet from camera:
			/*
			if (_Socket) {
				AProjectile* const NewProjectile = World->SpawnActor<AProjectile>
					(_ProjectileClass, _PlayerCharacter->GetPlayerCamera()->GetComponentTransform(), ActorSpawnParams);
			}
			*/
		}
	}
	/*Make noise*/
	_PlayerCharacter->MakeNoise(1.0f, GetOwner()->Instigator, GetOwner()->GetActorLocation());
}



