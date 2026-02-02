// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Goals/Goal.h"
#include "Goal_Idle.generated.h"

/**
 * 
 */
UCLASS()
class UEGOAP_API UGoal_Idle : public UGoal
{
	GENERATED_BODY()

public:
	UGoal_Idle() {
		Priority = 0;
	}

	bool IsSatisfied(const FWorldState& WorldState) const override;
	
};
