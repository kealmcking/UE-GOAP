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
	/** Finds a lowest-cost action sequence from StartState to a state satisfying Goal; writes it to OutPlan. */
	bool BuildPlan(
		const FWorldState& StartState,
		const UGoal* Goal,
		const TArray<UAction*>& Actions,
		TArray<UAction*>& OutPlan
	);
	
};
