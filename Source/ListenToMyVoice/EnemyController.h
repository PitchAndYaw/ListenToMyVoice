#pragma once


#include "AIController.h"
#include "EnemyController.generated.h"


UCLASS()
class LISTENTOMYVOICE_API AEnemyController : public AAIController {
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UAISenseConfig_Sight* _SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAISenseConfig_Hearing* _HearingConfig;

    AEnemyController(const FObjectInitializer& OI);

    virtual void Possess(APawn* InPawn) override;

protected:
    class APlayerCharacter* _TargetPawn;

    UFUNCTION()
    void PerceptionUpdated(TArray<AActor*> Actors);

    //UFUNCTION()
    //void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	void ApplySenses(float SightRange, float LoseSightRadius, float VisionAngleDegrees, float HearingRange);
};
