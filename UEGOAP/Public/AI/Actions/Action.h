// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Agent/WorldState.h"
#include "UObject/Object.h"
#include "Action.generated.h"

class AAgent;

/**
 * 
 */
UCLASS()
class UEGOAP_API UAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float Cost = 1.f;

	TMap<FName, int32> Preconditions;
	TMap<FName, int32> Effects;

	virtual bool CanExecute(const FWorldState& WorldState) const;
	virtual bool Execute(class AAgent* Agent);

	virtual bool IsComplete() const;
	
};
