#pragma once

#include "CoreMinimal.h"
#include "AI/Goals/Goal.h"
#include "Goal_StayFed.generated.h"

UCLASS()
class UEGOAP_API UGoal_StayFed : public UGoal
{
	GENERATED_BODY()

public:
	UGoal_StayFed() {
		Priority = 10;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal")
	int32 HungerThreshold = 40;

	virtual bool IsSatisfied(const FWorldState& WorldState) const override;
	
};
