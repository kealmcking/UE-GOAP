#include "AI/Actor/PineTreeActor.h"

APineTreeActor::APineTreeActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APineTreeActor::BeginPlay()
{
	Super::BeginPlay();
}

void APineTreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WoodAmount == 0) {
		SetLifeSpan(1.0f);
	}
}

int32 APineTreeActor::Gather(int32 Amount)
{
	if (WoodAmount <= 0) return 0;
	int32 Taken = FMath::Min(Amount, WoodAmount);
	WoodAmount -= Taken;
	return Taken;
}

int32 APineTreeActor::GetAvailableAmount() const
{
	return FMath::Max(0, WoodAmount);
}

