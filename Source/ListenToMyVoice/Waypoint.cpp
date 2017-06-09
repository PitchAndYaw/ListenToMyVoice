// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "Waypoint.h"


// Sets default values
AWaypoint::AWaypoint()
{
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));

}

// Called when the game starts or when spawned
void AWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
}


