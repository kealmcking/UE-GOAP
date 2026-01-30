#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CampfireActor.generated.h"

UCLASS()
class UEGOAP_API ACampfireActor : public AActor
{
	GENERATED_BODY()
	
public:
	ACampfireActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};
