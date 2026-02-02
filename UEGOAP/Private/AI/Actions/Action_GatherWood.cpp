// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Actions/Action_GatherWood.h"
#include "AI/Actor/Interfaces/Choppable.h"
#include "AI/Agent/Agent.h"
#include "EngineUtils.h"
#include "Navigation/PathFollowingComponent.h"

UAction_GatherWood::UAction_GatherWood()
{
	Cost = 1.0f;

	AcceptanceRadius = 150.f;

	Effects.Add("CarriedWood", 5);

	Preconditions.Add("Energy", 29);
}

bool UAction_GatherWood::CanExecute(const FWorldState& WorldState) const
{
	if (!UAction::CanExecute(WorldState))
		return false;

	return true;
}

bool UAction_GatherWood::Execute(AAgent* Agent)
{
	if (!Agent || !TargetActor)
		return false;

	if (!AgentController)
		AgentController = Cast<AAIController>(Agent->GetController());

	UPathFollowingComponent* PFC = AgentController ? AgentController->GetPathFollowingComponent() : nullptr;
	
	if (!bHasArrived)
	{
		const bool bAtTree = CheckArrival(Agent);
		const bool bPathIdle = PFC && PFC->GetStatus() == EPathFollowingStatus::Idle;
		
		if (!bAtTree && !bIsMoving)
		{
			StartMoveTo(Agent, TargetActor);
			return true;
		}
		
		if (!bAtTree && !bPathIdle)
		{
			return true;
		}
		
		bIsMoving = false;
		bHasArrived = true;
	}
	
	float DeltaTime = Agent->GetWorld() ? Agent->GetWorld()->GetDeltaSeconds() : 0.016f;
	TimeSpentGatheringWood += DeltaTime;
	if (TimeSpentGatheringWood < GatherWoodDuration)
		return true;

	IChoppable* Choppable = Cast<IChoppable>(TargetActor);
	if (!Choppable || Choppable->GetAvailableAmount() < 1)
	{
		Agent->LastDepletedWoodSource = TargetActor;
		if (Agent->WorldState.GetValue("CarriedWood") >= TargetWoodAmount)
		{
			bHasFoundWood = true;
			return true;
		}
		return false;
	}
	
	const int32 Taken = Choppable->Gather(1);
	if (Taken < 1)
	{
		Agent->LastDepletedWoodSource = TargetActor;
		if (Agent->WorldState.GetValue("CarriedWood") >= TargetWoodAmount)
		{
			bHasFoundWood = true;
			return true;
		}
		return false;
	}
	
	Agent->CurrentWoodSource = TargetActor;

	const int32 CurrentWood = Agent->WorldState.GetValue("CarriedWood");
	const int32 NewWood = FMath::Clamp(CurrentWood + Taken, 0, 100);
	Agent->WorldState.SetValue("CarriedWood", NewWood);

	const int32 CurrentHunger = Agent->WorldState.GetValue("Hunger");
	Agent->WorldState.SetValue("Hunger", FMath::Clamp(CurrentHunger + 1, 0, 100));

	const int32 CurrentEnergy = Agent->WorldState.GetValue("Energy");
	Agent->WorldState.SetValue("Energy", FMath::Clamp(CurrentEnergy - 1, 0, 100));

	TimeSpentGatheringWood = 0.f;
	
	if (NewWood >= TargetWoodAmount)
	{
		bHasFoundWood = true;
	}

	return true;
}

bool UAction_GatherWood::IsComplete() const
{
	return bHasFoundWood;
}

bool UAction_GatherWood::Setup(AAgent* Agent)
{
	if (!Agent) return false;

	AgentController = Cast<AAIController>(Agent->GetController());

	AActor* Closest = nullptr;
	float MinDistance = TNumericLimits<float>::Max();

	UWorld* World = Agent->GetWorld();
	if (!World) return false;

	for (TActorIterator<AActor> It(Agent->GetWorld()); It; ++It) {
		if (!It->GetClass()->ImplementsInterface(UChoppable::StaticClass()))
			continue;
		if (Agent->LastDepletedWoodSource && *It == Agent->LastDepletedWoodSource)
			continue;
		if (IChoppable* Choppable = Cast<IChoppable>(*It)) {
			if (Choppable->GetAvailableAmount() < 1)
				continue;
		}

		float Dist = FVector::Dist(Agent->GetActorLocation(), It->GetActorLocation());
		if (Dist < MinDistance) {
			MinDistance = Dist;
			Closest = *It;
		}
	}

	if (Closest)
	{
		TargetActor = Closest;
		return true;
	}

	if (Agent->LastDepletedWoodSource)
		Agent->LastDepletedWoodSource = nullptr;
	return false;
}

bool UAction_GatherWood::CheckArrival(AAgent* Agent)
{
	if (!Agent || !TargetActor)
		return false;

	FVector AgentXY = Agent->GetActorLocation();
	FVector TargetXY = TargetActor->GetActorLocation();
	AgentXY.Z = 0.f;
	TargetXY.Z = 0.f;
	float Dist2D = FVector::Dist(AgentXY, TargetXY);
	if (Dist2D <= AcceptanceRadius) {
		if (AgentController)
			AgentController->StopMovement();
		bIsMoving = false;
		bUseManualMovement = false;
		return true;
	}
	return false;
}

void UAction_GatherWood::ResetForPlan()
{
	UAction::ResetForPlan();
	bHasFoundWood = false;
	bHasArrived = false;
	TimeSpentGatheringWood = 0.f;
}

UAction* UAction_GatherWood::Clone(UObject* Outer) const
{
	UAction_GatherWood* NewAction = NewObject<UAction_GatherWood>(Outer, GetClass());
	NewAction->AcceptanceRadius = AcceptanceRadius;
	NewAction->ConsumeAmount = ConsumeAmount;
	NewAction->Cost = Cost;
	NewAction->Preconditions = Preconditions;
	NewAction->Effects = Effects;
	NewAction->GatherWoodDuration = GatherWoodDuration;
	NewAction->TargetWoodAmount = TargetWoodAmount;
	return NewAction;
}
