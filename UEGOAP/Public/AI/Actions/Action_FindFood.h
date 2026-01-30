#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"
#include "Action_FindFood.generated.h"

UCLASS()
class UEGOAP_API UAction_FindFood : public UAction
{
	GENERATED_BODY()

public:
	UAction_FindFood();

	virtual bool CanExecute(const FWorldState& WorldState) const override;

	virtual bool Execute(class AAgent* Agent) override;

	virtual bool IsComplete() const override;

	virtual bool Setup(AAgent* Agent) override;

	virtual bool CheckArrival(class AAgent* Agent) override;

	virtual void ResetForPlan() override;

	virtual bool SuppressesHungerDegradation() const override { return true; }

private:
	bool bHasFoundFood;
};
