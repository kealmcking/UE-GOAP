// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Stockable.generated.h"

UENUM()
enum class StockableType
{
	None,
	Wood,
	Food
};

UINTERFACE(MinimalAPI)
class UStockable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UEGOAP_API IStockable
{
	GENERATED_BODY()

public:
	virtual int32 Stock(int32 Amount) = 0;
	virtual int32 Gather(int32 Amount) = 0;

	virtual int32 GetMaxStorage() = 0;
	virtual int32 GetCurrentStorage() = 0;

	virtual StockableType WhatToStock() const { return StockableType::None; }
};
