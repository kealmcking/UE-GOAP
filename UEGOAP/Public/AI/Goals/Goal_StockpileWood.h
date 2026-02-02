#pragma once

#include "CoreMinimal.h"
#include "AI/Goals/Goal.h"
#include "Goal_StockpileWood.generated.h"

UCLASS()
class UEGOAP_API UGoal_StockpileWood : public UGoal
{
    GENERATED_BODY()

public:
    UGoal_StockpileWood() { Priority = 20; }

    bool IsSatisfied(const FWorldState& WorldState) const override;

};
