// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Components/SphereComponent.h"
#include "Sensor.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTargetChanged);

UCLASS()
class UEGOAP_API USensorComponent : public USceneComponent
{
	GENERATED_BODY()

	USensorComponent();

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* SphereCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fields")
	float DetectionRadius = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fields")
	float TimerInterval = 1.f;

	FOnTargetChanged OnTargetChanged;

	FVector GetTargetPosition() const;
	void UpdateTargetPosition(AActor* InTarget = nullptr);
	bool bIsTargetInRange() const;

	AActor* Target;
	FVector TargetsLastKnownPosition;

	FTimerHandle CountdownTimerHandle;

	void OnTimerFinished();

	void DrawDebugRange() const;

	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};
