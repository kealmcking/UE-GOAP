#include "AI/Actions/Action_StockWood.h"
#include "AI/Actor/Interfaces/Stockable.h"
#include "AI/Agent/Agent.h"
#include "EngineUtils.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"

UAction_StockWood::UAction_StockWood()
{
    Cost = 1.0f;
    AcceptanceRadius = 150.f;

    Preconditions.Add("CarriedWood", 1);

    Effects.Add("CarriedWood", 0);
}

bool UAction_StockWood::CanExecute(const FWorldState& WorldState) const
{
    return UAction::CanExecute(WorldState);
}

bool UAction_StockWood::Execute(AAgent* Agent)
{
    if (!Agent || !TargetActor)
        return false;

    if (!AgentController)
        AgentController = Cast<AAIController>(Agent->GetController());

    if (!bIsMoving) {
        if (!StartMoveTo(Agent, TargetActor))
            return false;
        return true;
    }

    UPathFollowingComponent* PFC = AgentController ? AgentController->GetPathFollowingComponent() : nullptr;
    if (PFC && PFC->GetStatus() == EPathFollowingStatus::Idle) {
        bIsMoving = false;

        IStockable* Stockable = Cast<IStockable>(TargetActor);
        if (!Stockable)
            return false;

        int32 Carried = Agent->WorldState.GetValue("CarriedWood");
        int32 Added = Stockable->Stock(Carried);

        Agent->WorldState.SetValue("CarriedWood", 0);
        int32 Stored = Agent->WorldState.GetValue("WoodStock");
        Agent->WorldState.SetValue("WoodStock", Stored + Added);

        bHasStockedWood = true;
        return true;
    }
    return true;
}

bool UAction_StockWood::IsComplete() const
{
    return bHasStockedWood;
}

bool UAction_StockWood::Setup(AAgent* Agent)
{
    if (!Agent)
        return false;

    AgentController = Cast<AAIController>(Agent->GetController());

    AActor* Closest = nullptr;
    float MinDistance = TNumericLimits<float>::Max();

    UWorld* World = Agent->GetWorld();
    if (!World)
        return false;

    for (TActorIterator<AActor> It(World); It; ++It) {
        if (!It->GetClass()->ImplementsInterface(UStockable::StaticClass()))
            continue;

        IStockable* Stockable = Cast<IStockable>(*It);
        if (!Stockable || Stockable->WhatToStock() != StockableType::Wood)
            continue;

        AmountToStock = FMath::Min(
            Agent->WorldState.GetValue("CarriedWood"),
            Stockable->GetMaxStorage() - Stockable->GetCurrentStorage()
        );

        if (AmountToStock <= 0)
            continue;

        float Dist = FVector::Dist(Agent->GetActorLocation(), It->GetActorLocation());
        if (Dist < MinDistance) {
            MinDistance = Dist;
            Closest = *It;
        }
    }

    if (Closest) {
        TargetActor = Closest;
        return true;
    }

    return false;
}

bool UAction_StockWood::CheckArrival(AAgent* Agent)
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

void UAction_StockWood::ResetForPlan()
{
    UAction::ResetForPlan();
    bHasStockedWood = false;
    AmountToStock = 0;
}

UAction* UAction_StockWood::Clone(UObject* Outer) const
{
    UAction_StockWood* NewAction = NewObject<UAction_StockWood>(Outer, GetClass());
    NewAction->AcceptanceRadius = AcceptanceRadius;
    NewAction->ConsumeAmount = ConsumeAmount;
    NewAction->Cost = Cost;
    NewAction->Preconditions = Preconditions;
    NewAction->Effects = Effects;
    return NewAction;
}
