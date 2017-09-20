// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "Gun.h"
#include "Projectile.h"
#include "EnemyCharacter.h"


// Sets default values
AProjectile::AProjectile() {
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bReplicateMovement = true;
    bAlwaysRelevant = true;

    _Damage = 0;
    // Use a sphere as a simple collision representation
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->SetCollisionProfileName("BlockAll");
    CollisionComp->InitSphereRadius(0.09f);
    CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

    // Players can't walk on it
    CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
    CollisionComp->CanCharacterStepUpOn = ECB_No;
    CollisionComp->SetNotifyRigidBodyCollision(true);

    // Set as root component
    RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 10000.0;
    ProjectileMovement->MaxSpeed = 10000.0;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0;
    ProjectileMovement->bConstrainToPlane = true;

    // Die after 3 seconds by default
    InitialLifeSpan = 3.0f;
}

void AProjectile::OnHit(UPrimitiveComponent* Hitcomp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
    if (Enemy) {
        Enemy->SERVER_TakeDamage(0);
        Destroy(true);
    }
}
