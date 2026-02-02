// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Stockable.h"
#include "GameFramework/Actor.h"
#include "WoodStockpileActor.generated.h"

UCLASS()
class UEGOAP_API AWoodStockpileActor : public AActor, public IStockable
{
	GENERATED_BODY()
	
public:	
	AWoodStockpileActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStorage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentStorage;

	virtual int32 GetCurrentStorage();
	virtual int32 GetMaxStorage();

	virtual int32 Stock(int32 Amount);
	virtual int32 Gather(int32 Amount);

	virtual StockableType WhatToStock() const {
		return StockableType::Wood;
	}

};
