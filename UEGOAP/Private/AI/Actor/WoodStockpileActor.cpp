// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Actor/WoodStockpileActor.h"

// Sets default values
AWoodStockpileActor::AWoodStockpileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWoodStockpileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWoodStockpileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


int32 AWoodStockpileActor::GetCurrentStorage()
{
	return CurrentStorage;
}

int32 AWoodStockpileActor::GetMaxStorage()
{
	return MaxStorage;
}

int32 AWoodStockpileActor::Stock(int32 Amount)
{
	const int32 SpaceAvailable = MaxStorage - CurrentStorage;
	const int32 ToAdd = FMath::Min(Amount, SpaceAvailable);
	CurrentStorage += ToAdd;
	return ToAdd;
}

int32 AWoodStockpileActor::Gather(int32 Amount)
{
	return CurrentStorage -= Amount;
}



