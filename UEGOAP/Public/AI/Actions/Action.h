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
	float AcceptanceRadius = 0.f;

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

	virtual bool Setup(AAgent* Agent);
	virtual bool CanExecute(const FWorldState& WorldState) const;
	virtual bool Execute(class AAgent* Agent);
	virtual bool IsComplete() const;

	virtual bool StartMoveTo(AAgent* Agent, AActor* Target);
	virtual bool CheckArrival(AAgent* Agent);

	virtual void ResetForPlan();
	
	/** Called when action is interrupted/aborted to release any held resources (beds, reservations, etc.) */
	virtual void Cleanup(AAgent* Agent);
	
	/** Creates a copy of this action for use in a plan (actions are cloned to avoid shared state issues) */
	virtual UAction* Clone(UObject* Outer) const;
	
	virtual bool SuppressesEnergyDegradation() const { return false; }
	virtual bool SuppressesHungerDegradation() const { return false; }
	void TickManualMovement(class AAgent* Agent, float DeltaTime);
};
