#include "AI/Actor/CampfireActor.h"

ACampfireActor::ACampfireActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACampfireActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACampfireActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

