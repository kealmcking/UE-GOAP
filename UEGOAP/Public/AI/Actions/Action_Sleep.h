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
	
	virtual void Cleanup(AAgent* Agent) override;
	
	virtual UAction* Clone(UObject* Outer) const override;

	virtual bool SuppressesEnergyDegradation() const override { return bIsAtBed; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	int32 RestoreAmountPerTick = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	float RestoreIntervalSeconds = 0.5f;

	/** Only consider sleeping when energy is below this (do not sleep until under 40). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	int32 SleepWhenUnder = 40;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	int32 RestUntilEnergy = 70;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rest")
	float GiveUpOnBedAfterSeconds = 4.f;

private:
	bool bHasSlept = false;
	bool bIsAtBed = false;
	float TimeUntilNextRest = 0.f;
	float TimeTryingToReachBed = 0.f;
	bool bGiveUpOnBed = false;
};
