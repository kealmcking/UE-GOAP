// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Planner/Planner.h"
#include "AI/Planner/Node.h"
#include <vector>

bool UPlanner::BuildPlan(const FWorldState& StartState, const UGoal* Goal, const TArray<UAction*>& Actions, TArray<UAction*>& OutPlan)
{
	// Empty List of Nodes
	std::vector<FNode> OpenSet;
	
	// Empty List of World States
	std::vector<FWorldState> ClosedSet;

	FNode StartNode;
	StartNode.WorldState = StartState;
	StartNode.Cost = 0;

	OpenSet.push_back(StartNode);



	while (!OpenSet.empty()) {
		auto BestIt = OpenSet.begin();
		float MinCost = FLT_MAX;
		
		for (auto It = OpenSet.begin(); It != OpenSet.end(); ++It) {
			if (It->Cost < MinCost) {
				MinCost = It->Cost;
				BestIt = It;
			}
		}

		FNode Current = *BestIt;
		OpenSet.erase(BestIt);

		if (Goal->IsSatisfied(Current.WorldState)) {
			OutPlan = Current.ActionPath;
			return true;
		}

		ClosedSet.push_back(Current.WorldState);

		for (UAction* Action : Actions) {
			if (Action->CanExecute(Current.WorldState)) {
				FWorldState NewState = Current.WorldState;

				for (auto Effect : Action->Effects) {
					NewState.Values.Add(Effect);
				}
			}
		}
	}

	return false;
}
