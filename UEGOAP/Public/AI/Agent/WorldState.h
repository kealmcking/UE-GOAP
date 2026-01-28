// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldState.generated.h"

/**
 * 
 */
USTRUCT()
struct UEGOAP_API FWorldState
{
	GENERATED_BODY()

public:
	TMap<FName, int32> Values;

	int32 GetValue(FName Key) const;
	void SetValue(FName Key, int32 Value);
	bool Meets(FName Key, int32 Value) const;
	bool Meets(const TMap<FName, int32>& Conditions) const;


	bool operator==(const FWorldState& Other) const
	{
		return Values.OrderIndependentCompareEqual(Other.Values);
	}

};
