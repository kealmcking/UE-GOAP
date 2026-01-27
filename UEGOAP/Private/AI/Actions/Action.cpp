// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Actions/Action.h"

bool UAction::CanExecute(const FWorldState& WorldState) const
{
	return false;
}

bool UAction::Execute(AGOAPAgent* Agent)
{
	return false;
}
