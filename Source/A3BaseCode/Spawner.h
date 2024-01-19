// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EvolutionManager.h"
#include "AsteroidCluster.h"
#include "Spawner.generated.h"

UCLASS()
class A3BASECODE_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Entities")
		float MaxHarvesterShipCount = 300;
	
	UPROPERTY(EditAnywhere, Category = "Entities")
		float MaxPirateShipCount = 20;
	
	UPROPERTY(EditAnywhere, Category = "Entities")
		float MaxAsteroidCount = 5;

	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<ABoid> HarvesterShipClass;

	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<ABoid> PirateShipClass;

	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AAsteroidCluster> AsteroidClusterClass;

	int NumHarvesterShips = 0;
	int NumPirateShips = 0;
	TArray<AAsteroidCluster*> AsteroidClusters;

	UPROPERTY() TArray<FBoidInfo> DeadHarvesterData;
	UPROPERTY() TArray<FBoidInfo> DeadPirateData;

	UPROPERTY()
	UEvolutionManager* EvolutionController = nullptr;

	void SpawnHarvesterShip();
	void SpawnPirateShip();

};
