#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"
#include "Action_Sleep.generated.h"

UCLASS()
class UEGOAP_API UAction_Sleep : public UAction
{
	GENERATED_BODY()

public:
	UAction_Sleep();

	virtual bool CanExecute(const FWorldState& WorldState) const override;

	virtual bool Execute(class AAgent* Agent) override;

	virtual bool IsComplete() const override;

	virtual bool Setup(AAgent* Agent) override;

	virtual void ResetForPlan() override;

	virtual bool SuppressesEnergyDegradation() const override { return true; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	int32 RestoreAmountPerTick = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	float RestoreIntervalSeconds = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	int32 TotalRestToRestore = 40;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	float GiveUpOnBedAfterSeconds = 4.f;

private:
	bool bHasSlept;
	int32 RestoredSoFar = 0;
	float TimeUntilNextRest = 0.f;
	float TimeTryingToReachBed = 0.f;
	bool bGiveUpOnBed = false;
};
