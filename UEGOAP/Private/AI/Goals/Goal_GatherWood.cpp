// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Goals/Goal_GatherWood.h"

bool UGoal_GatherWood::IsSatisfied(const FWorldState& WorldState) const
{
	return WorldState.GetValue("CarriedWood") >= 5;
}
