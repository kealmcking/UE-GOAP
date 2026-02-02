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

	UPROPERTY()
	AActor* LastDepletedWoodSource = nullptr;

	UPROPERTY()
	AActor* CurrentWoodSource = nullptr;

	UPROPERTY()
	UGoal* CurrentGoal = nullptr;

	void RequestPlan();

	UGoal* SelectTopGoal() const;

	void ClearRegeneratedSources();

private:
	float HungerAcummulationAccumulator = 0.f;
	float EnergyDegradationAccumulator = 0.f;
	float SourceCleanupAccumulator = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Degradation")
	float EnergyDegradationIntervalSeconds = 5.f;


	UPROPERTY(EditDefaultsOnly, Category = "Degradation")
	float HungerAccumulationIntervalSeconds = 5.f;

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGOAPDebugWidget> DebugWidgetClass;
};
