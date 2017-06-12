// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "BTTask_NewWaypoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

#include "EnemyCharacter.h"
#include "Waypoint.h"


UBTTask_NewWaypoint::UBTTask_NewWaypoint()
{
	BlackboardKey.SelectedKeyName = FName("Waypoint");
	BlackboardKey.SelectedKeyType = UBlackboardKeyType_Vector::StaticClass();
	if (GetWorld() && GetWorld()->AreActorsInitialized()) {
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypoint::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
			DefaultWaypoint = Cast<AWaypoint>(FoundActors[0]);
	}
	
}

EBTNodeResult::Type UBTTask_NewWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	AEnemyCharacter* Self = Cast<AEnemyCharacter>(OwnerComp.GetBlackboardComponent()->
		GetValueAsObject(FName("SelfActor")));
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->
		GetValueAsObject(FName("TargetPawn")));

		if (Self && DefaultWaypoint)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName,
				DefaultWaypoint->NextWaypoint->GetActorLocation());
			DefaultWaypoint = DefaultWaypoint->NextWaypoint;
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("TargetPawn"), Target);
			return EBTNodeResult::Type::Succeeded;
		}

		return EBTNodeResult::Type::Failed;
}