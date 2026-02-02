#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"
#include "Action_StockWood.generated.h"

UCLASS()
class UEGOAP_API UAction_StockWood : public UAction
{
    GENERATED_BODY()

public:
    UAction_StockWood();

    virtual bool CanExecute(const FWorldState& WorldState) const override;
    virtual bool Execute(class AAgent* Agent) override;
    virtual bool IsComplete() const override;
    virtual bool Setup(class AAgent* Agent) override;
    virtual bool CheckArrival(class AAgent* Agent) override;
    virtual void ResetForPlan() override;
    virtual UAction* Clone(UObject* Outer) const override;

private:
    bool bHasStockedWood = false;
    int32 AmountToStock = 0;
};
