// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Planner/Planner.h"
#include "AI/Planner/Node.h"
#include <vector>

bool UPlanner::BuildPlan(const FWorldState& StartState, const UGoal* Goal, const TArray<UAction*>& Actions, TArray<UAction*>& OutPlan)
{
	OutPlan.Reset();

	TArray<FNode> OpenSet;
	TArray<FWorldState> ClosedSet;

	FNode StartNode;
	StartNode.WorldState = StartState;
	StartNode.Cost = 0.f;

	OpenSet.Add(StartNode);

	while (OpenSet.Num() > 0) {
		OpenSet.Sort([](const FNode& A, const FNode& B) {
			return A.Cost < B.Cost;
			});

		FNode Current = OpenSet[0];
		OpenSet.RemoveAt(0);

		if (Goal->IsSatisfied(Current.WorldState)) {
			OutPlan = Current.ActionPath;
			return true;
		}

		ClosedSet.Add(Current.WorldState);

		for (UAction* Action : Actions) {
			if (!Action || !Action->CanExecute(Current.WorldState)) {
				continue;
			}

			FWorldState NewState = Current.WorldState;

			for (const auto& Effect : Action->Effects) {
				NewState.SetValue(Effect.Key, Effect.Value);
			}

			if (ClosedSet.Contains(NewState)) {
				continue;
			}

			FNode NewNode;
			NewNode.WorldState = NewState;
			NewNode.Cost = Current.Cost + Action->Cost;
			NewNode.ActionPath = Current.ActionPath;
			NewNode.ActionPath.Add(Action);

			OpenSet.Add(NewNode);
		}
	}
	return false;
}
