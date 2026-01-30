#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h" 
#include "AIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AI/Navigation/NavigationTypes.h" 
#include "AI/Agent/WorldState.h"
#include "UObject/Object.h"
#include "Action.generated.h"

class AAgent;

UCLASS(Abstract)
class UEGOAP_API UAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action|Movement")
	float AcceptanceRadius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action|Movement")
	int32 ConsumeAmount = 1;

	UPROPERTY()
	AActor* TargetActor = nullptr;

	UPROPERTY()
	AAIController* AgentController = nullptr;

	UPROPERTY()
	bool bIsMoving = false;

	UPROPERTY()
	bool bUseManualMovement = false;

	UPROPERTY(EditDefaultsOnly)
	float Cost = 1.f;

	TMap<FName, int32> Preconditions;
	TMap<FName, int32> Effects;

	/** Override to pick targets and prepare the action before execution. */
	virtual bool Setup(AAgent* Agent);
	/** True when the world state meets all preconditions. */
	virtual bool CanExecute(const FWorldState& WorldState) const;
	/** Runs the action for one tick; return true to keep running. */
	virtual bool Execute(class AAgent* Agent);
	/** True when the action has finished. */
	virtual bool IsComplete() const;

	virtual bool StartMoveTo(AAgent* Agent, AActor* Target);
	virtual bool CheckArrival(AAgent* Agent);

	/** Clears movement and target state so the action can run again in a new plan. */
	virtual void ResetForPlan();
	virtual bool SuppressesEnergyDegradation() const { return false; }
	virtual bool SuppressesHungerDegradation() const { return false; }
	void TickManualMovement(class AAgent* Agent, float DeltaTime);
};
