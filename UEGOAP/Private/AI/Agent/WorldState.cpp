// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Agent/WorldState.h"

int32 FWorldState::GetValue(FName Key) const 
{
	// Change to grab value based on key
	return 0;
}

void FWorldState::SetValue(FName Key, int32 Value)
{
}

bool FWorldState::Meets(const TMap<FName, int32>& Conditions) const
{
	return false;
}

