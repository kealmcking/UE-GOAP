#include "AI/Actions/Action_Sleep.h"
#include "AI/Actor/Interfaces/Restable.h"
#include "AI/Agent/Agent.h"
#include "EngineUtils.h"

/**
 * Effect adds Energy; no preconditions (CanExecute gates on low energy).
 */
UAction_Sleep::UAction_Sleep() : bHasSlept(false), RestoredSoFar(0), TimeUntilNextRest(0.f)
{
	Cost = 1.0f;

	Effects.Add("Energy", 40);
}

/**
 * True when energy is below threshold so the planner only considers Sleep when tired.
 */
bool UAction_Sleep::CanExecute(const FWorldState& WorldState) const
{
	const int32 EnergyThreshold = 70;
	return WorldState.GetValue("Energy") < EnergyThreshold;
}

/**
 * Moves to a bed if one was reserved; restores energy over time (at bed or in place if movement gave up).
 */
bool UAction_Sleep::Execute(AAgent* Agent)
{
	if (!Agent)
		return false;

	const float DeltaTime = Agent->GetWorld() ? Agent->GetWorld()->GetDeltaSeconds() : 0.016f;

	if (TargetActor && !bGiveUpOnBed)
	{
		if (CheckArrival(Agent))
		{
		}
		else
		{
			TimeTryingToReachBed += DeltaTime;
			if (TimeTryingToReachBed >= GiveUpOnBedAfterSeconds)
			{
				bGiveUpOnBed = true;
				if (IRestable* Restable = Cast<IRestable>(TargetActor))
					Restable->Release(Agent);
			}
			else
			{
				if (!bIsMoving)
				{
					if (!AgentController)
						AgentController = Cast<AAIController>(Agent->GetController());
					if (!AgentController)
						return false;
					if (!StartMoveTo(Agent, TargetActor))
						return false;
					return true;
				}
				if (bUseManualMovement)
					TickManualMovement(Agent, DeltaTime);
				return true;
			}
		}
	}

	TimeUntilNextRest -= DeltaTime;
	if (TimeUntilNextRest <= 0.f)
	{
		const int32 ToRestore = FMath::Min(RestoreAmountPerTick, TotalRestToRestore - RestoredSoFar);
		if (ToRestore > 0)
		{
			RestoredSoFar += ToRestore;
			Agent->WorldState.SetValue("Energy", FMath::Clamp(Agent->WorldState.GetValue("Energy") + ToRestore, 0, 100));
			if (TargetActor && !bGiveUpOnBed)
			{
				if (IRestable* Restable = Cast<IRestable>(TargetActor))
					Restable->Rest(ToRestore);
			}
		}
		TimeUntilNextRest = RestoreIntervalSeconds;
	}

	if (RestoredSoFar >= TotalRestToRestore)
	{
		if (TargetActor)
		{
			if (IRestable* Restable = Cast<IRestable>(TargetActor))
				Restable->Release(Agent);
		}
		bHasSlept = true;
	}
		

	return true;
}

/**
 * True once TotalRestToRestore energy has been restored.
 */
bool UAction_Sleep::IsComplete() const
{
	return bHasSlept;
}

/**
 * Finds the closest available restable, reserves it, and sets it as the movement target.
 */
bool UAction_Sleep::Setup(AAgent* Agent)
{
	if (!Agent || !Agent->GetWorld())
		return true;

	AActor* ClosestAvailableBed = nullptr;
	float ClosestDistance = TNumericLimits<float>::Max();

	for (TActorIterator<AActor> It(Agent->GetWorld()); It; ++It)
	{
		if (!It->GetClass()->ImplementsInterface(URestable::StaticClass()))
			continue;

		IRestable* Restable = Cast<IRestable>(*It);
		if (!Restable)
			continue;

		if (!Restable->IsAvailable())
			continue;

		float Distance = FVector::Dist(
			Agent->GetActorLocation(),
			It->GetActorLocation()
		);

		if (Distance < ClosestDistance) {
			ClosestDistance = Distance;
			ClosestAvailableBed = *It;
		}

	}

	if (!ClosestAvailableBed)
		return false;

	IRestable* ClosestRestable = Cast<IRestable>(ClosestAvailableBed);
	if (!ClosestRestable)
		return false;

	if (!ClosestRestable->TryReserve(Agent))
		return false;

	TargetActor = ClosestAvailableBed;
	return true;
}

/**
 * Releases the reserved bed (if any) and resets sleep state for a new plan.
 */
void UAction_Sleep::ResetForPlan()
{
	UAction::ResetForPlan();

	if (TargetActor) {
		if (IRestable* Restable = Cast<IRestable>(TargetActor)) {
			Restable->Release(nullptr);
		}
	}

	TargetActor = nullptr;
	bHasSlept = false;
	RestoredSoFar = 0;
	TimeUntilNextRest = 0.f;
	TimeTryingToReachBed = 0.f;
	bGiveUpOnBed = false;
}

