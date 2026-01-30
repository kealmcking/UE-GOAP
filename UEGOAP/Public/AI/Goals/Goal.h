#pragma once

#include "CoreMinimal.h"
#include "AI/Agent/WorldState.h"
#include "UObject/Object.h"
#include "Goal.generated.h"

UCLASS(Abstract)
class UEGOAP_API UGoal : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	int32 Priority;

	/** True when the world state meets this goal. */
	virtual bool IsSatisfied(const FWorldState& WorldState) const PURE_VIRTUAL(UGoal::IsSatisfied, return false;);
};
