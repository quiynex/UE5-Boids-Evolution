// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "Harvester.h"
#include "Pirate.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	EvolutionController = NewObject<UEvolutionManager>();
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	for(int i = 0; i < MaxHarvesterShipCount; i++)
	{
		SpawnHarvesterShip();
	}

	for(int i = 0; i < MaxPirateShipCount; i++)
	{
		SpawnPirateShip();
	}

	for(int i = 0; i < MaxAsteroidCount; i++)
	{
		FVector SpawnLocation(FMath::RandRange(0.0f, 5000.0f),FMath::RandRange(-2500.0f, 2500.0f),FMath::RandRange(-2500.0f, 2500.0f));
		if(AsteroidClusterClass)
		{
			AsteroidClusters.Add(Cast<AAsteroidCluster>(GetWorld()->SpawnActor(AsteroidClusterClass, &SpawnLocation)));
		}
	}
	
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(NumHarvesterShips == 0)
	{
		while(NumHarvesterShips < MaxHarvesterShipCount)
		{
			SpawnHarvesterShip();
		}
	}
	else if (NumHarvesterShips < MaxHarvesterShipCount * 0.2f) {
		TArray<AActor*> AliveHarvesterActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarvester::StaticClass(), AliveHarvesterActors);

		TArray<ABoid*> AliveHarvesters;
		for (AActor*& Ele: AliveHarvesterActors) {
			AliveHarvesters.Add(Cast<ABoid>(Ele));
		}

		auto HarvesterData = EvolutionController->SortByFitness(AliveHarvesters, DeadHarvesterData);
		auto NewHarvesterGenes = EvolutionController->BreedingMutation(HarvesterData, MaxHarvesterShipCount * 0.5f, true);

		// empty dead boids array
		DeadHarvesterData.Empty();

		EvolutionController->PopulationSelection(AliveHarvesters, AliveHarvesters.Num());
		NumHarvesterShips -= AliveHarvesters.Num();

		for (auto& Ele: NewHarvesterGenes) {
			FVector SpawnLocation(FMath::RandRange(500.0f, 4500.0f),FMath::RandRange(-2000.0f, 2000.0f),FMath::RandRange(-2000.0f, 2000.0f));
			if (HarvesterShipClass)
			{
				ABoid *SpawnedShip = Cast<ABoid>(GetWorld()->SpawnActor(HarvesterShipClass, &SpawnLocation, &FRotator::ZeroRotator));
				if (SpawnedShip)
				{
					SpawnedShip->Spawner = this;
					EvolutionController->ApplyToBoid(Ele, SpawnedShip, true);
					NumHarvesterShips++;
				}
			}
		}
	}


	if(NumPirateShips == 0)
	{
		while(NumPirateShips < MaxPirateShipCount)
		{
			SpawnPirateShip();
		}
	}
	else if (NumPirateShips < MaxPirateShipCount * 0.2f) {
		TArray<AActor*> AlivePirateActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APirate::StaticClass(), AlivePirateActors);

		TArray<ABoid*> AlivePirates;
		for (AActor*& Ele: AlivePirateActors) {
			AlivePirates.Add(Cast<ABoid>(Ele));
		}

		auto PirateData = EvolutionController->SortByFitness(AlivePirates, DeadPirateData);
		auto NewPirateGenes = EvolutionController->BreedingMutation(PirateData, MaxPirateShipCount * 0.5f);

		// empty dead boids array
		DeadPirateData.Empty();

		EvolutionController->PopulationSelection(AlivePirates, AlivePirates.Num());
		NumPirateShips -= AlivePirates.Num();

		for (auto& Ele: NewPirateGenes) {
			FVector SpawnLocation(FMath::RandRange(500.0f, 4500.0f),FMath::RandRange(-2000.0f, 2000.0f),FMath::RandRange(-2000.0f, 2000.0f));
			if (PirateShipClass)
			{
				ABoid *SpawnedShip = Cast<ABoid>(GetWorld()->SpawnActor(PirateShipClass, &SpawnLocation, &FRotator::ZeroRotator));
				if (SpawnedShip)
				{
					SpawnedShip->Spawner = this;
					EvolutionController->ApplyToBoid(Ele, SpawnedShip);
					NumPirateShips++;
				}
			}
		}
	}

}

void ASpawner::SpawnHarvesterShip()
{
	FVector SpawnLocation(FMath::RandRange(500.0f, 4500.0f),FMath::RandRange(-2000.0f, 2000.0f),FMath::RandRange(-2000.0f, 2000.0f));
	if(HarvesterShipClass)
	{
		ABoid* SpawnedShip = Cast<ABoid>(GetWorld()->SpawnActor(HarvesterShipClass, &SpawnLocation,&FRotator::ZeroRotator));
		if(SpawnedShip)
		{
			SpawnedShip->Spawner = this;
			EvolutionController->InitBoidGene(SpawnedShip, true);
			NumHarvesterShips++;
		}
	}
}

void ASpawner::SpawnPirateShip()
{
	FVector SpawnLocation(FMath::RandRange(500.0f, 4500.0f),FMath::RandRange(-2000.0f, 2000.0f),FMath::RandRange(-2000.0f, 2000.0f));
	if(PirateShipClass)
	{
		ABoid* SpawnedShip = Cast<ABoid>(GetWorld()->SpawnActor(PirateShipClass, &SpawnLocation,&FRotator::ZeroRotator));
		if(SpawnedShip)
		{
			SpawnedShip->Spawner = this;
			EvolutionController->InitBoidGene(SpawnedShip);
			NumPirateShips++;
		}
	}
}
