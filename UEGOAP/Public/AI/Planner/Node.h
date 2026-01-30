#pragma once

#include "CoreMinimal.h"
#include "AI/Agent/WorldState.h"
#include "AI/Actions/Action.h"

struct FNode
{
	FWorldState WorldState;
	TArray<UAction*> ActionPath;
	float Cost;

	bool operator<(const FNode& Other) const { return Cost > Other.Cost; }
};
