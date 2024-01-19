// Fill out your copyright notice in the Description page of Project Settings.


#include "AsteroidCluster.h"

#include "NiagaraFunctionLibrary.h"

// Sets default values
AAsteroidCluster::AAsteroidCluster()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	AsteroidHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	AsteroidHitbox->SetupAttachment(RootComponent);

}

float AAsteroidCluster::RemoveGold()
{
	if(ResourceAmount > 0)
	{
		ResourceAmount-=AmountToTake;
		return AmountToTake;
	}
	return 0;
}

// Called when the game starts or when spawned
void AAsteroidCluster::BeginPlay()
{
	Super::BeginPlay();
	if(AsteroidParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(AsteroidParticle, AsteroidHitbox, TEXT("Hitbox"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
}

// Called every frame
void AAsteroidCluster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ResourceAmount <= 0)
	{
		SetActorLocation(FVector(FMath::RandRange(0.0f, 5000.0f),FMath::RandRange(-2500.0f, 2500.0f),FMath::RandRange(-2500.0f, 2500.0f)));
		ResourceAmount = 1000;
	}

}

