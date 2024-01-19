// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "AsteroidCluster.generated.h"

UCLASS()
class A3BASECODE_API AAsteroidCluster : public AActor
{
	GENERATED_BODY()
	
public:	
	AAsteroidCluster();

	UPROPERTY(EditAnywhere)
		USphereComponent* AsteroidHitbox;
	
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* AsteroidParticle;

	UPROPERTY(VisibleAnywhere)
		float ResourceAmount = 1000;

	float RemoveGold();

	float AmountToTake = 0.01;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
