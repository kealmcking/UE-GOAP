#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AI/Actions/Action.h"
#include "AI/Goals/Goal.h"
#include "Planner.generated.h"

UCLASS()
class UEGOAP_API UPlanner : public UObject
{
	GENERATED_BODY()

public:
	/** 
	 * Finds a lowest-cost action sequence from StartState to a state satisfying Goal.
	 * Actions are CLONED so each plan step has independent state.
	 * @param ActionOuter The outer object for cloned actions (typically the Executor)
	 */
	bool BuildPlan(
		const FWorldState& StartState,
		const UGoal* Goal,
		const TArray<UAction*>& Actions,
		TArray<UAction*>& OutPlan,
		UObject* ActionOuter
	);
	
private:
	/** Applies an action's effects to a world state for planning simulation. */
	void ApplyEffects(const UAction* Action, FWorldState& State);
};
