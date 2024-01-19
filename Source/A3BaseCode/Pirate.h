// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"
#include "Pirate.generated.h"

/**
 * 
 */
UCLASS()
class A3BASECODE_API APirate : public ABoid
{
	GENERATED_BODY()

public:

public:
	APirate();

	bool bStay = false;
	float StayTime = 0.0f;
	float MaxStayTime = 5.0f;

	virtual FVector AvoidBoids(TArray<AActor*> Flock) override;
	virtual FVector VelocityMatching(TArray<AActor*> Flock) override;
	virtual FVector FlockCentering(TArray<AActor*> Flock) override;
	virtual FVector Migration() override;
	virtual FVector AvoidObstacle() override;

	virtual void FlightPath(float DeltaTime) override;
	
	virtual void OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
