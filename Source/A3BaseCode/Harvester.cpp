// Fill out your copyright notice in the Description page of Project Settings.


#include "Harvester.h"

#include "Spawner.h"
#include "Engine/StaticMeshActor.h"

AHarvester::AHarvester()
{
	MinSpeed = 500;
	MaxSpeed = 1000;
}

FVector AHarvester::AvoidBoids(TArray<AActor *> Flock)
{
	// avoid boids from the same class and pirates
	double Threshhold = 400;
	FVector Result(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Boid")) && Ele != this))
			continue;

		if (Ele->ActorHasTag(TEXT("Harvester"))) {
			double Distance = 0.0;
			if ((Distance = FVector::Dist(Ele->GetActorLocation(), GetActorLocation())) < Threshhold) {
				Result += GetActorLocation() - Ele->GetActorLocation();
				FlockSize++;
			}
		}
		else {
			Result += GetActorLocation() - Ele->GetActorLocation();
			FlockSize++;
		}
	}

	if (FlockSize == 0)
		return FVector::ZeroVector;
	return Result / FlockSize;
}

FVector AHarvester::VelocityMatching(TArray<AActor*> Flock)
{
	FVector CenterVelocity(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Harvester")) && Ele != this))
			continue;
		
		ABoid* EleBoid = Cast<ABoid>(Ele);
		CenterVelocity += EleBoid->BoidVelocity;
		FlockSize++;
	}

	if (FlockSize == 0)
		return FVector::ZeroVector;
	return CenterVelocity / FlockSize - BoidVelocity;
}

FVector AHarvester::FlockCentering(TArray<AActor*> Flock)
{
	FVector Center(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Harvester")) && Ele != this))
			continue;
		Center += Ele->GetActorLocation();
		FlockSize++;
	}

	if (FlockSize == 0)
		return FVector::ZeroVector;

	// double Threshhold = 0.0;
	// FVector AveragedCenter = Center / FlockSize;
	// if (FVector::Dist(AveragedCenter, GetActorLocation()) > Threshhold)
	// 	return FVector::ZeroVector;
	return Center / FlockSize - GetActorLocation();
}

FVector AHarvester::Migration()
{
    return Super::Migration();
}

FVector AHarvester::AvoidObstacle()
{
	return Super::AvoidObstacle();
}

void AHarvester::OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Invincibility < 0)
	{
		if(OtherActor && OtherActor != this && OverlappedComponent->GetName().Equals(TEXT("Boid Collision Component")))
		{
			AAsteroidCluster* Asteroids = Cast<AAsteroidCluster>(OtherActor);
			if(Asteroids)
			{
				CollisionAsteroids = Asteroids;
				return;
			}

			ABoid* OtherShip = Cast<ABoid>(OtherActor);
			if(OtherShip && OtherComponent->GetName().Equals(TEXT("Boid Collision Component")))
			{
				Spawner->NumHarvesterShips--;

				if (OtherShip->ActorHasTag(TEXT("Pirate")))
					Fitness *= 0.5f;
				else
					Fitness -= Fitness * 0.25f;

				Spawner->DeadHarvesterData.Add({ Gene, Fitness, GetName() });
				Destroy();
				return;
			}

			AStaticMeshActor* Wall = Cast<AStaticMeshActor>(OtherActor);
			if(Wall)
			{
				Spawner->NumHarvesterShips--;
				Fitness -= Fitness * 0.25f;

				Spawner->DeadHarvesterData.Add({ Gene, Fitness, GetName() });
				Destroy();
				return;
			}
		}
	}
}

void AHarvester::OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	AAsteroidCluster* Asteroids = Cast<AAsteroidCluster>(OtherActor);
	if(Asteroids)
	{
		CollisionAsteroids = nullptr;
	}
}
