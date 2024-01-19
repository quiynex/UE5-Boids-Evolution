// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsteroidCluster.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "EvolutionManager.h"
#include "Boid.generated.h"

class ASpawner;
class UGene;

UCLASS()
class A3BASECODE_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoid();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BoidMesh;
	UPROPERTY(EditAnywhere)
		USphereComponent* BoidCollision;
	UPROPERTY(EditAnywhere)
		USphereComponent* PerceptionSensor;

	UPROPERTY(VisibleAnywhere)
		float ResourcesCollected = 0.0f;

	FVector BoidVelocity;
	float BoidSpeed = 0.0f;

	UPROPERTY()	UGene* Gene;

	FRotator CurrentRotation;
	void UpdateMeshRotation();

	virtual FVector AvoidBoids(TArray<AActor*> Flock);

	virtual FVector VelocityMatching(TArray<AActor*> Flock);

	virtual FVector FlockCentering(TArray<AActor*> Flock);

	virtual FVector AvoidObstacle();

	virtual FVector Migration();

	virtual void FlightPath(float DeltaTime);

	TTuple<AActor*, float> IsObstacleAhead();

	TArray<FVector> AsteroidForces;

	float MinSpeed = 300.0f;
	float MaxSpeed = 600.0f;

	UPROPERTY(EditAnywhere)
	float VelocityMatchingStrength = 0.1f;

	UPROPERTY(EditAnywhere)
	float AvoidBoidsStrength = 1.0f;

	UPROPERTY(EditAnywhere)
	float CenteringStrength = 0.05f;

	UPROPERTY(EditAnywhere)
	float ObstacleAvoidanceStrength = 10.0f;

	UPROPERTY(EditAnywhere)
	float AsteroidStrength = 1.0f;

	float Invincibility = 1.0f;

	TArray<FVector> AvoidanceSensors;
	float SensorRadius = 600.0f;
	int NumSensors = 100;

	float GoldenRatio = (1.0f + FMath::Sqrt(5.0f))/2;
	
	AAsteroidCluster* CollisionAsteroids;
	ASpawner* Spawner;

	float Fitness = 0.0f;

	UFUNCTION()
		virtual void OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor,  UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,  bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

};
