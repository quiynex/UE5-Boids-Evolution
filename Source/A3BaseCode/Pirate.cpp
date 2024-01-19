// Fill out your copyright notice in the Description page of Project Settings.


#include "Pirate.h"

#include "Spawner.h"
#include "Harvester.h"
#include "Engine/StaticMeshActor.h"

APirate::APirate()
{
	MinSpeed = 650.0f;
	MaxSpeed = 650.0f;
}

FVector APirate::AvoidBoids(TArray<AActor *> Flock)
{
	// avoid boids from the same class
	double Threshhold = 400;
	FVector Result(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Pirate")) && Ele != this))
			continue;

		double Distance = 0.0;
		if ((Distance = FVector::Dist(Ele->GetActorLocation(), GetActorLocation())) < Threshhold) {
			Result += GetActorLocation() - Ele->GetActorLocation();
			FlockSize++;
		}
	}

	if (FlockSize == 0)
		return FVector::ZeroVector;
	return Result / FlockSize;
}

FVector APirate::VelocityMatching(TArray<AActor*> Flock)
{
	FVector CenterVelocity(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Pirate")) && Ele != this))
			continue;
		
		ABoid* EleBoid = Cast<ABoid>(Ele);
		CenterVelocity += EleBoid->BoidVelocity;
		FlockSize++;
	}

	if (FlockSize == 0)
		return FVector::ZeroVector;
	return CenterVelocity / FlockSize - BoidVelocity;
}

FVector APirate::FlockCentering(TArray<AActor*> Flock)
{
	FVector Center(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Pirate")) && Ele != this))
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

FVector APirate::Migration()
{
    TArray<AActor*> Flock;
	PerceptionSensor->GetOverlappingActors(Flock, AHarvester::StaticClass());

	if (Flock.Num() == 0)
		return Super::Migration();

	AActor *NearestGoal = nullptr;
	double NearestDist = 99999999.9;
	double NewDist = 0.0;
	for (AActor *&Ele : Flock)
	{
		if ((NewDist = FVector::Dist(Ele->GetActorLocation(), GetActorLocation())) < NearestDist)
		{
			NearestDist = NewDist;
			NearestGoal = Ele;
		}
	}

	return NearestGoal->GetActorLocation() - GetActorLocation();
}

FVector APirate::AvoidObstacle()
{
	return Super::AvoidObstacle();
}

void APirate::FlightPath(float DeltaTime)
{
	if (!bStay) {
		Super::FlightPath(DeltaTime);
	}
	else {
		StayTime += DeltaTime;
		if (StayTime > MaxStayTime) {
			bStay = false;
			StayTime = 0.0f;
		}
	}
}

void APirate::OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != this && Invincibility <= 0 && OverlappedComponent->GetName().Equals(TEXT("Boid Collision Component")))
	{
		APirate* OtherPirate = Cast<APirate>(OtherActor);
		if(OtherPirate && OtherComponent->GetName().Equals(TEXT("Boid Collision Component")))
		{
			Spawner->NumPirateShips--;
			Fitness -= Fitness * 0.25f;

			Spawner->DeadPirateData.Add({ Gene, Fitness, GetName() });
			Destroy();
			return;
		}

		AHarvester* OtherHarvester = Cast<AHarvester>(OtherActor);
		if (OtherHarvester && OtherComponent->GetName().Equals(TEXT("Boid Collision Component")))
		{
			ResourcesCollected += OtherHarvester->ResourcesCollected;
			Fitness += OtherHarvester->ResourcesCollected;

			OtherHarvester->ResourcesCollected = 0.0f;
			bStay = true;
			StayTime = 0.0f;
		}

		AStaticMeshActor* Wall = Cast<AStaticMeshActor>(OtherActor);
		if(Wall)
		{
			Spawner->NumPirateShips--;
			Fitness -= Fitness * 0.25f;

			Spawner->DeadPirateData.Add({ Gene, Fitness, GetName() });
			Destroy();
			return;
		}
	}
}
