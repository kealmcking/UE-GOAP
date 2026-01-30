#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "GOAPDebugWidget.generated.h"

class AAgent;

UCLASS()
class UEGOAP_API UGOAPDebugWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Sets the agent whose world state, goal, and plan are displayed. */
	void InitializeWidget(AAgent* InAgent);

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ConsolidatedDebugText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* GoalText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlanText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ActionText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WorldStateText;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY()
	AAgent* ObservedAgent;
	
};
