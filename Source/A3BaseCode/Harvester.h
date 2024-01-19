// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"
#include "Harvester.generated.h"

/**
 * 
 */
UCLASS()
class A3BASECODE_API AHarvester : public ABoid
{
	GENERATED_BODY()

public:
	AHarvester();

	virtual FVector AvoidBoids(TArray<AActor*> Flock) override;
	virtual FVector VelocityMatching(TArray<AActor*> Flock) override;
	virtual FVector FlockCentering(TArray<AActor*> Flock) override;
	virtual FVector Migration() override;
	virtual FVector AvoidObstacle() override;
	
	virtual void OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex) override;
	
	
};
