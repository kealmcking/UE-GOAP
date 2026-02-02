#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"
#include "Action_Eat.generated.h"

UCLASS()
class UEGOAP_API UAction_Eat : public UAction
{
	GENERATED_BODY()

public:
	UAction_Eat();

	virtual bool CanExecute(const FWorldState& WorldState) const override;

	virtual bool Execute(class AAgent* Agent) override;

	virtual bool IsComplete() const override;

	virtual bool Setup(AAgent* Agent) override;

	virtual void ResetForPlan() override;
	
	virtual UAction* Clone(UObject* Outer) const override;

	virtual bool SuppressesHungerDegradation() const override { return true; }

	UPROPERTY(EditDefaultsOnly, Category = "Eat")
	float EatDuration = 2.f;

private:
	bool bHasEaten = false;
	float TimeSpentEating = 0.f;
};
