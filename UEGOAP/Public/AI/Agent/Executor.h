#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AI/Actions/Action.h"
#include "Executor.generated.h"

UCLASS()
class UEGOAP_API UExecutor : public UObject
{
	GENERATED_BODY()

	UExecutor();

public:
	/** Runs the current action each tick; aborts plan if stuck or preconditions fail. */
	void TickExecution(AAgent* Agent, float DeltaTime);
	/** Replaces the current plan with a new action sequence. */
	void SetPlan(const TArray<UAction*>& NewPlan);

	bool HasActivePlan() const { return CurrentPlan.Num() > 0; }

	/** Returns the action currently being executed, or nullptr. */
	UAction* GetCurrentAction() const;

private:

	UPROPERTY()
	TArray<UAction*> CurrentPlan;

	int32 CurrentActionIndex;

	int32 LastLoggedActionIndex = -1;
	float TimeOnCurrentAction = 0.f;
	int32 ConsecutiveExecuteFalse = 0;

	static const float StuckTimeoutSeconds;
};
