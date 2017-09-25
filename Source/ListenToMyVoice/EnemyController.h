// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"


#include "AIController.h"
#include "EnemyController.generated.h"


UCLASS()
class LISTENTOMYVOICE_API AEnemyController : public AAIController {
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAISenseConfig_Sight* _SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Hearing* _HearingConfig;

    AEnemyController(const FObjectInitializer& OI);

    virtual void Possess(APawn* InPawn) override;

protected:
    class APlayerCharacter* _TargetPawn;

    UFUNCTION()
    void PerceptionUpdated(TArray<AActor*> Actors);

private:
	void ApplySenses(float SightRange, float LoseSightRadius, float VisionAngleDegrees, float HearingRange);
};
