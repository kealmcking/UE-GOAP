// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Actions/Action_Eat.h"
#include "AI/Agent/Agent.h"

UAction_Eat::UAction_Eat() : bHasEaten(false)
{
	Cost = 1.0f;

	// Agent must have food
	Preconditions.Add("HasFood", 1);

	// When eaten, reduce hunger, remove food
	Effects.Add("Hunger", -40);
	Effects.Add("HasFood", 0);
}

bool UAction_Eat::CanExecute(const FWorldState& WorldState) const
{
	return UAction::CanExecute(WorldState);
}

bool UAction_Eat::Execute(AAgent* Agent)
{
	if (!Agent)
		return false;

	for (const TPair<FName, int32>& Effect : Effects) {
		int32 Current = Agent->WorldState.GetValue(Effect.Key);
		int32 NewValue = Current + Effect.Value;
		Agent->WorldState.SetValue(Effect.Key, NewValue);
	}

	bHasEaten = true;
	return true;
}

bool UAction_Eat::IsComplete() const
{
	return bHasEaten;
}
