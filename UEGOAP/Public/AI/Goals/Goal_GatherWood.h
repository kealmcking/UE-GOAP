// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Goals/Goal.h"
#include "Goal_GatherWood.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UGoal_GatherWood : public UGoal
{
	GENERATED_BODY()

public:
	UGoal_GatherWood() {
		Priority = 5;
	}
	
	bool IsSatisfied(const FWorldState& WorldState) const override;
};
