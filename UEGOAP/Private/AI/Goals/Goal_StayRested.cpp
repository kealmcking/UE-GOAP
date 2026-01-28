// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Goals/Goal_StayRested.h"

bool UGoal_StayRested::IsSatisfied(const FWorldState& WorldState) const
{
	return WorldState.Meets("Energy", EnergyThreshold);
}
