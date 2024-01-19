// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "Spawner.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABoid::ABoid()
{
	PrimaryActorTick.bCanEverTick = true;

	BoidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boid Mesh Component"));
	BoidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoidMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	BoidCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Boid Collision Component"));
	BoidCollision->SetCollisionObjectType(ECC_Pawn);
	BoidCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoidCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoidCollision->SetSphereRadius(100.0f);

	RootComponent = BoidMesh;
	BoidCollision->SetupAttachment(RootComponent);

	BoidCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoid::OnHitboxOverlapBegin);
	BoidCollision->OnComponentEndOverlap.AddDynamic(this, &ABoid::OnHitboxOverlapEnd);
	
	PerceptionSensor = CreateDefaultSubobject<USphereComponent>(TEXT("Perception Sensor Component"));
	PerceptionSensor->SetupAttachment(RootComponent);
	PerceptionSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PerceptionSensor->SetCollisionResponseToAllChannels(ECR_Ignore);
	PerceptionSensor->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PerceptionSensor->SetSphereRadius(1000.0f);
	
	BoidVelocity = FVector::ZeroVector;
	
	AvoidanceSensors.Empty();
	
	FVector SensorDirection;

	for (int i = 0; i < NumSensors; ++i)
	{
		float Yaw = 2 * UKismetMathLibrary::GetPI() * GoldenRatio * i;
		float Pitch = FMath::Acos(1 - (2 * float(i) / NumSensors));
		
		SensorDirection.X = FMath::Cos(Yaw)*FMath::Sin(Pitch);
		SensorDirection.Y = FMath::Sin(Yaw)*FMath::Sin(Pitch);
		SensorDirection.Z = FMath::Cos(Pitch);

		AvoidanceSensors.Emplace(SensorDirection);
	}

}

void ABoid::BeginPlay()
{
	Super::BeginPlay();

	FRotator SpawnRotation(FMath::RandRange(-180,180),FMath::RandRange(-180,180),FMath::RandRange(-180,180));
	SetActorRotation(SpawnRotation);
	//BoidMesh->AddLocalRotation(FRotator(0,180,0));
	CurrentRotation = GetActorRotation();

	BoidVelocity = GetActorForwardVector();
	BoidVelocity*= FMath::RandRange(MinSpeed, MaxSpeed);
}

void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FlightPath(DeltaTime);

	UpdateMeshRotation();

	if(CollisionAsteroids)
	{
		float CollectedAmount = CollisionAsteroids->RemoveGold();
		ResourcesCollected += CollectedAmount;
		Fitness += CollectedAmount;
	}

	if(Invincibility > 0)
	{
		Invincibility-= DeltaTime;
	}

	Fitness += DeltaTime;
}

void ABoid::UpdateMeshRotation()
{
	CurrentRotation = FMath::RInterpTo(CurrentRotation, GetActorRotation(), GetWorld()->DeltaTimeSeconds, 7.0f);
	SetActorRotation(CurrentRotation);
}

FVector ABoid::AvoidBoids(TArray<AActor*> Flock)
{
	// check https://github.com/IshanManchanda/Boids

	// avoid boids from the same class
	double Threshhold = 400;
	FVector Result(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Boid")) && Ele != this))
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

FVector ABoid::VelocityMatching(TArray<AActor*> Flock)
{
	FVector CenterVelocity(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Boid")) && Ele != this))
			continue;
		
		ABoid* EleBoid = Cast<ABoid>(Ele);
		CenterVelocity += EleBoid->BoidVelocity;
		FlockSize++;
	}

	if (FlockSize == 0)
		return FVector::ZeroVector;
	return CenterVelocity / FlockSize - BoidVelocity;
}

FVector ABoid::FlockCentering(TArray<AActor*> Flock)
{
	FVector Center(0);
	int32 FlockSize = 0;
	for (AActor*& Ele: Flock) {
		if (!(Ele->ActorHasTag(TEXT("Boid")) && Ele != this))
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

FVector ABoid::AvoidObstacle()
{
	auto [Obstacle, Distance] = IsObstacleAhead();

	if (!Obstacle || Obstacle->ActorHasTag("Boid") || Obstacle->ActorHasTag("Asteroid"))
		return FVector::ZeroVector;

	if (Distance > 300)
		return FVector::ZeroVector;

	return -GetActorForwardVector() * Distance;
}

FVector ABoid::Migration()
{
    AAsteroidCluster* NearestGoal = nullptr;
	double NearestDist = 99999999.9;
	double NewDist = 0.0;
	for (AAsteroidCluster*& Ele: Spawner->AsteroidClusters) {
		if ((NewDist = FVector::Dist(Ele->GetActorLocation(), GetActorLocation())) < NearestDist) {
			NearestDist = NewDist;
			NearestGoal = Ele;
		}
	}

	if (!NearestGoal)
		return FVector::ZeroVector;
	return NearestGoal->GetActorLocation() - GetActorLocation();
}

void ABoid::FlightPath(float DeltaTime)
{
	// update boid position and rotation

	TArray<AActor*> PerceivedFlock;
	PerceptionSensor->GetOverlappingActors(PerceivedFlock);

	FVector Acceleration(0);
	Acceleration += AvoidBoids(PerceivedFlock) * AvoidBoidsStrength;
	Acceleration += VelocityMatching(PerceivedFlock) * VelocityMatchingStrength;
	Acceleration += FlockCentering(PerceivedFlock) * CenteringStrength;
	Acceleration += AvoidObstacle() * ObstacleAvoidanceStrength;
	Acceleration += Migration() * AsteroidStrength;

	BoidVelocity += Acceleration * DeltaTime;
	if (BoidVelocity.Size() < MinSpeed) {
		BoidVelocity.Normalize();
		BoidVelocity *= MinSpeed;
	}
	else if (BoidVelocity.Size() > MaxSpeed) {
		BoidVelocity.Normalize();
		BoidVelocity *= MaxSpeed;
	}

	//UE_LOG(LogTemp, Warning, TEXT("speed: %f"), BoidVelocity.Size());

	SetActorLocation(GetActorLocation() + BoidVelocity * DeltaTime);
	SetActorRotation(BoidVelocity.Rotation());
}

TTuple<AActor*, float> ABoid::IsObstacleAhead()
{
	if (AvoidanceSensors.Num() > 0)
	{
		FCollisionQueryParams TraceParameters;
		FHitResult Hit;
		
		GetWorld()->LineTraceSingleByChannel(Hit,this->GetActorLocation(),this->GetActorLocation() + GetActorForwardVector() * SensorRadius,ECC_GameTraceChannel1,TraceParameters);

		if (Hit.bBlockingHit)
		{
			TArray<AActor*> OverlapActors;
			BoidCollision->GetOverlappingActors(OverlapActors);
			for (AActor* OverlapActor : OverlapActors)
			{
				if (Hit.GetActor() == OverlapActor)
				{
					return {nullptr, 0.0f};
				}
			}
		}
		return {Hit.GetActor(), Hit.Distance};
	}
	return {nullptr, 0.0f};
}

void ABoid::OnHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABoid::OnHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	
}

