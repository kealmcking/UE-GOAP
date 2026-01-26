// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Senseable.generated.h"

UINTERFACE(MinimalAPI)
class USenseable : public UInterface
{
	GENERATED_BODY()
};


class UEGOAP_API ISenseable
{
	GENERATED_BODY()

public:
	virtual FVector GetSenseLocation() const = 0;

	virtual bool IsSenseActive() const { return true; }
};
