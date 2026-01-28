// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Goals/Goal_StayFed.h"

bool UGoal_StayFed::IsSatisfied(const FWorldState& WorldState) const {
	return WorldState.Meets("Hunger", HungerThreshold);
}

