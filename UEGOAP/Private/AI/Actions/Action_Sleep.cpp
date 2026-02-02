#include "AI/Actions/Action_Sleep.h"
#include "AI/Actor/Interfaces/Restable.h"
#include "AI/Agent/Agent.h"
#include "EngineUtils.h"
#include "Navigation/PathFollowingComponent.h"

/**
 * Effect adds Energy; no preconditions (CanExecute gates on low energy).
 */
UAction_Sleep::UAction_Sleep() : bHasSlept(false), TimeUntilNextRest(0.f)
{
	Cost = 1.0f;
	AcceptanceRadius = 10.f;
	SleepWhenUnder = 40;
	RestUntilEnergy = 70;

	Effects.Add("Energy", 40);
}

/**
 * True when we may start sleep (energy < 40) or when we are resting and may continue (energy < 70).
 * Uses "IsResting" world state instead of instance state to ensure deterministic planning.
 */
bool UAction_Sleep::CanExecute(const FWorldState& WorldState) const
{
	const int32 Energy = WorldState.GetValue("Energy");
	if (Energy < SleepWhenUnder)
		return true;
	const int32 IsResting = WorldState.GetValue("IsResting");
	if (IsResting > 0 && Energy < RestUntilEnergy)
		return true;
	return false;
}

/**
 * Moves to a bed if one was reserved; restores energy over time.
 */
bool UAction_Sleep::Execute(AAgent* Agent)
{
	if (!Agent)
		return false;

	const float DeltaTime = Agent->GetWorld() ? Agent->GetWorld()->GetDeltaSeconds() : 0.016f;

	AAIController* AIC = Agent ? Cast<AAIController>(Agent->GetController()) : nullptr;
	UPathFollowingComponent* PFC = AIC ? AIC->GetPathFollowingComponent() : nullptr;
	const bool bPathReachedGoal = (PFC && PFC->GetStatus() == EPathFollowingStatus::Idle);

	if (TargetActor && !bGiveUpOnBed)
	{
		const bool bAtBedByDistance = CheckArrival(Agent);
		const bool bAtBedByPath = bPathReachedGoal && (TimeTryingToReachBed >= 0.25f || bIsMoving || bIsAtBed);
		if (bAtBedByDistance || bAtBedByPath)
		{
			const bool bJustArrived = !bIsAtBed;
			bIsAtBed = true;
			if (bJustArrived)
			{
				Agent->WorldState.SetValue("IsResting", 1);
				TimeUntilNextRest = 0.f;
			}
			if (AgentController)
				AgentController->StopMovement();
			bIsMoving = false;
			bUseManualMovement = false;
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
					if (!StartMoveTo(Agent, TargetActor))
						return false;
					return true;
				}
				return true;
			}
		}
	}

	if (TargetActor && bIsAtBed)
	{
		const int32 CurrentEnergy = Agent->WorldState.GetValue("Energy");
		if (CurrentEnergy >= RestUntilEnergy)
		{
			if (IRestable* Restable = Cast<IRestable>(TargetActor))
				Restable->Release(Agent);
			Agent->WorldState.SetValue("IsResting", 0);
			bHasSlept = true;
		}
		else
		{
			TimeUntilNextRest -= DeltaTime;
			if (TimeUntilNextRest <= 0.f)
			{
				const int32 ToRestore = FMath::Min(RestoreAmountPerTick, RestUntilEnergy - CurrentEnergy);
				if (ToRestore > 0)
				{
					Agent->WorldState.SetValue("Energy", FMath::Clamp(CurrentEnergy + ToRestore, 0, 100));
					if (IRestable* Restable = Cast<IRestable>(TargetActor))
						Restable->Rest(ToRestore);
				}
				TimeUntilNextRest = RestoreIntervalSeconds;
			}
		}
	}
		

	return true;
}

/**
 * True once energy has reached RestUntilEnergy.
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
		return false;

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

	bHasSlept = false;
	bIsAtBed = false;
	TimeUntilNextRest = 0.f;
	TimeTryingToReachBed = 0.f;
	bGiveUpOnBed = false;
}

/**
 * Releases the bed reservation when the action is aborted.
 */
void UAction_Sleep::Cleanup(AAgent* Agent)
{
	if (TargetActor)
	{
		if (IRestable* Restable = Cast<IRestable>(TargetActor))
		{
			Restable->Release(Agent);
		}
	}
	
	if (Agent)
	{
		Agent->WorldState.SetValue("IsResting", 0);
	}
	
	UAction::Cleanup(Agent);
	
	bHasSlept = false;
	bIsAtBed = false;
	TimeUntilNextRest = 0.f;
	TimeTryingToReachBed = 0.f;
	bGiveUpOnBed = false;
}

/**
 * Creates a copy of this action with the same configuration.
 */
UAction* UAction_Sleep::Clone(UObject* Outer) const
{
	UAction_Sleep* NewAction = NewObject<UAction_Sleep>(Outer, GetClass());
	NewAction->AcceptanceRadius = AcceptanceRadius;
	NewAction->ConsumeAmount = ConsumeAmount;
	NewAction->Cost = Cost;
	NewAction->Preconditions = Preconditions;
	NewAction->Effects = Effects;
	NewAction->RestoreAmountPerTick = RestoreAmountPerTick;
	NewAction->RestoreIntervalSeconds = RestoreIntervalSeconds;
	NewAction->SleepWhenUnder = SleepWhenUnder;
	NewAction->RestUntilEnergy = RestUntilEnergy;
	NewAction->GiveUpOnBedAfterSeconds = GiveUpOnBedAfterSeconds;
	return NewAction;
}

