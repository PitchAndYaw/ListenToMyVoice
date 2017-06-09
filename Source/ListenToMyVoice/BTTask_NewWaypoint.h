// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Waypoint.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_NewWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class LISTENTOMYVOICE_API UBTTask_NewWaypoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_NewWaypoint();

	AWaypoint* DefaultWaypoint;
	TArray<AActor*> FoundActors;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
