#pragma once

#include "CoreMinimal.h"
#include "AI/Goals/Goal.h"
#include "Goal_StaySafe.generated.h"

UCLASS()
class UEGOAP_API UGoal_StaySafe : public UGoal
{
	GENERATED_BODY()

public:
	UGoal_StaySafe() {
		Priority = 10;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal")
	int32 ThreatThreshold = 0;

	virtual bool IsSatisfied(const FWorldState& WorldState) const override;
	
};
