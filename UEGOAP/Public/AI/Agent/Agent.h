#pragma once

#include "CoreMinimal.h"
#include "Debug/GOAPDebugWidget.h"
#include "GameFramework/Character.h"
#include "AI/Agent/WorldState.h"
#include "AI/Actions/Action.h"
#include "AI/Agent/Executor.h"
#include "AI/Planner/Planner.h"
#include "AI/Goals/Goal.h"
#include "Agent.generated.h"

UCLASS()
class UEGOAP_API AAgent : public ACharacter
{
	GENERATED_BODY()

public:
	AAgent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	FWorldState WorldState;

	UPROPERTY()
	TArray<UAction*> AvailableActions;

	UPROPERTY()
	TArray<UGoal*> Goals;

	UPROPERTY()
	UPlanner* Planner;

	UPROPERTY()
	UExecutor* Executor;

	UPROPERTY()
	AActor* LastDepletedFoodSource = nullptr;

	UPROPERTY()
	AActor* CurrentFoodSource = nullptr;

	/** Requests a new plan when none is active; resets and sets up actions then hands the sequence to the executor. */
	void RequestPlan();

	/** Returns the highest-priority goal that is not yet satisfied by the current world state. */
	UGoal* SelectTopGoal() const;

private:
	FTimerHandle PlanTimerHandle;

	float DegradationAccumulator = 0.f;

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGOAPDebugWidget> DebugWidgetClass;
};
