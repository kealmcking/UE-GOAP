// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Agent/WorldState.h"

int32 FWorldState::GetValue(FName Key) const 
{
	const int32* Found = Values.Find(Key);
	return Found ? *Found : 0;
}

void FWorldState::SetValue(FName Key, int32 Value)
{
	Values.Add(Key, Value);
}

bool FWorldState::Meets(FName Key, int32 Value) const
{
	const int32* Found = Values.Find(Key);
	return Found && *Found >= Value;
}

bool FWorldState::Meets(const TMap<FName, int32>& Conditions) const {
	for (const auto& Condition : Conditions) {
		if (!Meets(Condition.Key, Condition.Value)) {
			return false;
		}
	}
	return true;
}

