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

}

