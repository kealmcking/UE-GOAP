// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Goals/Goal_Idle.h"

bool UGoal_Idle::IsSatisfied(const FWorldState& WorldState) const
{
	return WorldState.GetValue("Wandered") >= 1;
}
