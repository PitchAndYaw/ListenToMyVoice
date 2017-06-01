// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Attack.generated.h"


UCLASS()
class LISTENTOMYVOICE_API UBTTask_Attack : public UBTTask_BlackboardBase {
    GENERATED_BODY()

public:
    UBTTask_Attack();

private:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
