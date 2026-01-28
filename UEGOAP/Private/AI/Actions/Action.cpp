// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Actions/Action.h"
#include "AI/Agent/Agent.h"


bool UAction::CanExecute(const FWorldState& WorldState) const
{
	for (const TPair<FName, int32>& Condition : Preconditions) {
		if (!WorldState.Meets(Condition.Key, Condition.Value)) {
			return false;
		}
	}
	return true;
}

bool UAction::Execute(AAgent* Agent)
{
	for (auto& Effect : Effects) {
		Agent->WorldState.SetValue(Effect.Key, Effect.Value);
	}

	return true;
}

bool UAction::IsComplete() const {
	return true;
}
