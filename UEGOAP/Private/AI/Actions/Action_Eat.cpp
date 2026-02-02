#include "AI/Actions/Action_Eat.h"
#include "AI/Actor/Interfaces/Edible.h"
#include "AI/Agent/Agent.h"
#include "EngineUtils.h"

/**
 * Sets preconditions (HasFood) and effects (reduce Hunger, clear HasFood).
 */
UAction_Eat::UAction_Eat() : bHasEaten(false)
{
	Cost = 1.0f;

	Preconditions.Add("HasFood", 1);

	Effects.Add("Hunger", -40);
	Effects.Add("HasFood", 0);
}

/**
 * True when the agent has food (set by FindFood).
 */
bool UAction_Eat::CanExecute(const FWorldState& WorldState) const
{
	return UAction::CanExecute(WorldState);
}

/**
 * Waits EatDuration seconds then consumes from CurrentFoodSource; reduces hunger and clears HasFood.
 */
bool UAction_Eat::Execute(AAgent* Agent)
{
	if (!Agent)
		return false;

	AActor* FoodTarget = Agent->CurrentFoodSource ? Agent->CurrentFoodSource : TargetActor;
	if (!FoodTarget && Agent->GetWorld())
	{
		AActor* Closest = nullptr;
		float MinDist = TNumericLimits<float>::Max();
		for (TActorIterator<AActor> It(Agent->GetWorld()); It; ++It)
		{
			if (!It->GetClass()->ImplementsInterface(UEdible::StaticClass()))
				continue;
			if (Agent->LastDepletedFoodSource && *It == Agent->LastDepletedFoodSource)
				continue;
			if (IEdible* Edible = Cast<IEdible>(*It))
			{
				if (Edible->GetAvailableAmount() < 1)
					continue;
			}
			float D = FVector::Dist(Agent->GetActorLocation(), It->GetActorLocation());
			if (D < MinDist)
			{
				MinDist = D;
				Closest = *It;
			}
		}
		FoodTarget = Closest;
	}
	if (!FoodTarget)
		return false;

	IEdible* Edible = Cast<IEdible>(FoodTarget);
	if (!Edible)
		return false;

	float DeltaTime = Agent->GetWorld() ? Agent->GetWorld()->GetDeltaSeconds() : 0.016f;
	TimeSpentEating += DeltaTime;
	if (TimeSpentEating < EatDuration)
		return true;

	int32 HungerBefore = Agent->WorldState.GetValue("Hunger");
	int32 Consumed = Edible->Consume(1);
	if (Consumed > 0)
	{
		int32 NewHunger = FMath::Clamp(HungerBefore - 40, 0, 100);
		Agent->WorldState.SetValue("Hunger", NewHunger);
		Agent->WorldState.SetValue("HasFood", 0);
		Agent->LastDepletedFoodSource = nullptr;
		Agent->CurrentFoodSource = nullptr;
		bHasEaten = true;
		return true;
	}

	Agent->LastDepletedFoodSource = FoodTarget;
	Agent->CurrentFoodSource = nullptr;
	Agent->WorldState.SetValue("HasFood", 0);
	return false;
}

/**
 * True once food was successfully consumed this plan.
 */
bool UAction_Eat::IsComplete() const
{
	return bHasEaten;
}

/**
 * No target needed here; Eat uses Agent->CurrentFoodSource set when FindFood completes.
 */
bool UAction_Eat::Setup(AAgent* Agent)
{
	if (!Agent || !Agent->GetWorld())
		return false;
	TargetActor = nullptr;
	return true;
}

/**
 * Clears eaten state so the action can run again in a new plan.
 */
void UAction_Eat::ResetForPlan()
{
	UAction::ResetForPlan();
	bHasEaten = false;
	TimeSpentEating = 0.f;
}

UAction* UAction_Eat::Clone(UObject* Outer) const
{
	UAction_Eat* NewAction = NewObject<UAction_Eat>(Outer, GetClass());
	NewAction->AcceptanceRadius = AcceptanceRadius;
	NewAction->ConsumeAmount = ConsumeAmount;
	NewAction->Cost = Cost;
	NewAction->Preconditions = Preconditions;
	NewAction->Effects = Effects;
	NewAction->EatDuration = EatDuration;
	return NewAction;
}
