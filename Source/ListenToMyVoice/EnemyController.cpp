// Fill out your copyright notice in the Description page of Project Settings.

#include "ListenToMyVoice.h"
#include "EnemyController.h"

#include "EnemyCharacter.h"
#include "PlayerCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"


AEnemyController::AEnemyController(const FObjectInitializer& OI) : Super(OI) {
    _SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	_HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component")));
    GetAIPerceptionComponent()->bAutoActivate = true;
    GetAIPerceptionComponent()->ConfigureSense(*_SightConfig);
	GetAIPerceptionComponent()->ConfigureSense(*_HearingConfig);
    GetAIPerceptionComponent()->SetDominantSense(_SightConfig->GetSenseImplementation());

    GetAIPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AEnemyController::PerceptionUpdated);
    //GetAIPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::TargetPerceptionUpdated);
}

void AEnemyController::Possess(APawn* InPawn) {
    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(InPawn);
    if (EnemyCharacter) {
        Super::Possess(InPawn);

        //EnemyCharacter->_BreathAudioComp->Play();

        RunBehaviorTree(Cast<AEnemyCharacter>(GetPawn())->_BehaviourTree);

		ApplySenses(EnemyCharacter->_SightRadius,
			EnemyCharacter->_LoseSightRadius,
			EnemyCharacter->_VisionAngleDegrees,
			EnemyCharacter->_HearingRange);
    }
}

void AEnemyController::ApplySenses(float SightRange, float LoseSightRadius, float VisionAngleDegrees, float HearingRange) {
	/* Sight */
    _SightConfig->SightRadius = SightRange;
    _SightConfig->LoseSightRadius = LoseSightRadius;
    _SightConfig->PeripheralVisionAngleDegrees = VisionAngleDegrees;
    _SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    _SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    _SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    GetAIPerceptionComponent()->ConfigureSense(*_SightConfig);
    GetAIPerceptionComponent()->SetDominantSense(_SightConfig->GetSenseImplementation());
	/* Hearing */
	_HearingConfig->HearingRange = HearingRange;
	_HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	_HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	_HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	GetAIPerceptionComponent()->ConfigureSense(*_HearingConfig);
}

void AEnemyController::PerceptionUpdated(TArray<AActor*> Actors) {
    /* Find Player */
    bool found = false;
    int i = 0;
    while (!found && i < Actors.Num()) {
        if (Actors[i]->IsA<APlayerCharacter>()) found = true;
        else i++;
    }

    if (found) {
        _TargetPawn = _TargetPawn ? nullptr : Cast<APlayerCharacter>(Actors[i]);
        if (Blackboard) {
            const UBlackboardData* BBAsset = Blackboard->GetBlackboardAsset();
            if (BBAsset) {
                const FBlackboard::FKey TargetKey = BBAsset->GetKeyID(FName("TargetPawn"));
                if (TargetKey != FBlackboard::InvalidKey) {
                    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKey, _TargetPawn);
                }
            }
        }
    }
}

//void AEnemyController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) {
//    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OnTargetPerceptionUpdated");
//
//    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Actor->GetName());
//}