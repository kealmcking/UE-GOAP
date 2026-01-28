// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Goals/Goal_StaySafe.h"

bool UGoal_StaySafe::IsSatisfied(const FWorldState& WorldState) const
{
	return WorldState.Meets("ThreatNearby", ThreatThreshold);
}
