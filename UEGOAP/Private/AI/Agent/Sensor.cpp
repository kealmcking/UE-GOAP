// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Agent/Sensor.h"
#include "AI/Agent/Senseable.h"
#include "DrawDebugHelpers.h"


USensorComponent::USensorComponent() {
	// Create Object
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));

	// Set up Collision Values
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionObjectType(ECC_WorldDynamic);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereCollider->SetGenerateOverlapEvents(true);

	// Set Size of Sphere
	SphereCollider->SetSphereRadius(DetectionRadius);
	
}

void USensorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SphereCollider) {
		SphereCollider->OnComponentBeginOverlap.AddDynamic(
			this, 
			&USensorComponent::OnOverlapBegin
		);

		SphereCollider->OnComponentEndOverlap.AddDynamic(
			this,
			&USensorComponent::OnOverlapEnd
		);
	}
	

	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimerHandle,
		this,
		&USensorComponent::OnTimerFinished,
		TimerInterval,
		false
	);
}

FVector USensorComponent::GetTargetPosition() const
{
	return Target ? Target->GetActorLocation() : FVector::ZeroVector;
}

void USensorComponent::UpdateTargetPosition(AActor*  InTarget)
{
	Target = InTarget;
	if (bIsTargetInRange() && (TargetsLastKnownPosition != GetTargetPosition() || TargetsLastKnownPosition != FVector::ZeroVector)) {
		TargetsLastKnownPosition = GetTargetPosition();
		OnTargetChanged.Broadcast();
	}
}

bool USensorComponent::bIsTargetInRange() const
{
	return GetTargetPosition() != FVector::ZeroVector;
}

void USensorComponent::OnTimerFinished()
{
	UpdateTargetPosition();
	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimerHandle,
		this,
		&USensorComponent::OnTimerFinished,
		TimerInterval,
		false
	);
}


// --------- DEBUG -----------
void USensorComponent::DrawDebugRange() const
{
	const FColor Color = bIsTargetInRange() ? FColor::Red : FColor::Green;

	DrawDebugSphere(
		GetWorld(),
		GetComponentLocation(),
		DetectionRadius,
		32,						// Segments
		Color,
		false,					// Persistent Lines
		-1.0f,					// Lifetime (one frame)
		0,						// Depth Priority
		2.0f					// Line Thickness
	);
}

void USensorComponent::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	if (!OtherActor->Implements<USenseable>())
		return;

	UpdateTargetPosition(OtherActor);

}

void USensorComponent::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	if (!OtherActor->Implements<USenseable>())
		return;
	UpdateTargetPosition();
}


