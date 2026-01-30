#include "Debug/GOAPDebugWidget.h"
#include "AI/Agent/Agent.h"
#include "AI/Agent/Executor.h"
#include "AI/Goals/Goal.h"

/**
 * Sets the agent whose world state, goal, and plan are shown in the widget.
 */
void UGOAPDebugWidget::InitializeWidget(AAgent* InAgent)
{
	ObservedAgent = InAgent;
}

/**
 * Updates the displayed world state, goal, and current action (ConsolidatedDebugText or separate text blocks).
 */
void UGOAPDebugWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ObservedAgent)
		return;

	const int32 Hunger = ObservedAgent->WorldState.GetValue("Hunger");
	const int32 Energy = ObservedAgent->WorldState.GetValue("Energy");
	const int32 HasFood = ObservedAgent->WorldState.GetValue("HasFood");
	UGoal* Goal = ObservedAgent->SelectTopGoal();
	const UAction* Current = ObservedAgent->Executor ? ObservedAgent->Executor->GetCurrentAction() : nullptr;
	const FString GoalStr = Goal ? Goal->GetName() : TEXT("None");
	const FString ActionStr = Current ? Current->GetClass()->GetName() : TEXT("None");

	if (ConsolidatedDebugText)
	{
		FString Consolidated = FString::Printf(TEXT("World: Hunger=%d  Energy=%d  HasFood=%d\nGoal: %s\nPlan: %s\nAction: %s"),
			Hunger, Energy, HasFood, *GoalStr, *ActionStr, *ActionStr);
		ConsolidatedDebugText->SetText(FText::FromString(Consolidated));
		return;
	}

	if (WorldStateText)
		WorldStateText->SetText(FText::FromString(FString::Printf(TEXT("Hunger: %d  |  Energy: %d  |  HasFood: %d"), Hunger, Energy, HasFood)));
	if (GoalText)
		GoalText->SetText(FText::FromString(GoalStr));
	if (PlanText)
		PlanText->SetText(FText::FromString(Current ? FString::Printf(TEXT("Plan: %s"), *ActionStr) : TEXT("Plan: None")));
	if (ActionText)
		ActionText->SetText(FText::FromString(ActionStr));
}
