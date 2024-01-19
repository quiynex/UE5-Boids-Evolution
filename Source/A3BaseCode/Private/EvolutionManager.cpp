// Fill out your copyright notice in the Description page of Project Settings.


#include "EvolutionManager.h"
#include "../Boid.h"
#include "Gene.h"

UEvolutionManager::UEvolutionManager()
{
}

UGene *UEvolutionManager::GenerateRandomGene(bool bIsHarvester)
{
    UGene* Result = NewObject<UGene>();
    Result->Genes[(uint8)EGeneKey::VelocityMatching] = FMath::RandRange(0.1f, 10.0f);
    Result->Genes[(uint8)EGeneKey::AvoidBoids] = FMath::RandRange(1.0f, 10000.0f);
    Result->Genes[(uint8)EGeneKey::Centering] = FMath::RandRange(0.01f, 0.1f);
    Result->Genes[(uint8)EGeneKey::ObstacleAvoidance] = FMath::RandRange(10.0f, 1000.0f);
    Result->Genes[(uint8)EGeneKey::AsteroidCluster] = FMath::RandRange(0.1f, 10.0f);

    if (bIsHarvester)
        Result->Genes[(uint8)EGeneKey::Speed] = FMath::RandRange(500.0f, 1000.0f);
    
    return Result;
}

void UEvolutionManager::ApplyToBoid(UGene* Gene, ABoid* Boid, bool bIsHarvester)
{
    Boid->VelocityMatchingStrength = Gene->Genes[(uint8)EGeneKey::VelocityMatching];
    Boid->AvoidBoidsStrength = Gene->Genes[(uint8)EGeneKey::AvoidBoids];
    Boid->CenteringStrength = Gene->Genes[(uint8)EGeneKey::Centering];
    Boid->ObstacleAvoidanceStrength = Gene->Genes[(uint8)EGeneKey::ObstacleAvoidance];
    Boid->AsteroidStrength = Gene->Genes[(uint8)EGeneKey::AsteroidCluster];

    if (bIsHarvester) {
        Boid->BoidSpeed = Gene->Genes[(uint8)EGeneKey::Speed];
        Boid->MinSpeed = Boid->BoidSpeed;
        Boid->MaxSpeed = Boid->BoidSpeed;
    }
    
    Boid->Gene = Gene;
}

TTuple<UGene *, UGene *> UEvolutionManager::BreedingMutationIndividual(UGene *ParentA, UGene *ParentB, bool bIsHarvester)
{
    UGene* ChildA = GenerateRandomGene(bIsHarvester);
    UGene* ChildB = GenerateRandomGene(bIsHarvester);

    for (uint8 i = 0; i < (uint8)EGeneKey::EnumSize; i++) {
        int32 Roll = FMath::RandRange(1, 100);
        if (Roll >= 1 && Roll <= 48) {
            ChildA->Genes[i] = ParentA->Genes[i];
            ChildB->Genes[i] = ParentB->Genes[i];
        }
        else if (Roll >= 49 && Roll <= 96) {
            ChildA->Genes[i] = ParentB->Genes[i];
            ChildB->Genes[i] = ParentA->Genes[i];
        }
    }

    return {ChildA, ChildB};
}

void UEvolutionManager::PrintStengths(UGene *Gene)
{
    UE_LOG(LogTemp, Warning, TEXT("VelocityMatching: %f"), Gene->Genes[(uint8)EGeneKey::VelocityMatching]);
    UE_LOG(LogTemp, Warning, TEXT("AvoidBoids: %f"), Gene->Genes[(uint8)EGeneKey::AvoidBoids]);
    UE_LOG(LogTemp, Warning, TEXT("Centering: %f"), Gene->Genes[(uint8)EGeneKey::Centering]);
    UE_LOG(LogTemp, Warning, TEXT("ObstacleAvoidance: %f"), Gene->Genes[(uint8)EGeneKey::ObstacleAvoidance]);
    UE_LOG(LogTemp, Warning, TEXT("AsteroidCluster: %f"), Gene->Genes[(uint8)EGeneKey::AsteroidCluster]);

    if (Gene->Genes[(uint8)EGeneKey::Speed] != 0.0f)
        UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), Gene->Genes[(uint8)EGeneKey::Speed]);
}

void UEvolutionManager::InitBoidGene(ABoid* Boid, bool IsHarvester)
{
    ApplyToBoid(GenerateRandomGene(IsHarvester), Boid, IsHarvester);
}

TArray<FBoidInfo> UEvolutionManager::SortByFitness(TArray<ABoid*>& AliveBoids, TArray<FBoidInfo>& DeadBoids)
{
    DeadBoids.Sort([](const FBoidInfo& A, const FBoidInfo& B) {
        return A.Fitness > B.Fitness;
    });

    AliveBoids.Sort([](const ABoid& A, const ABoid& B) {
        return A.Fitness > B.Fitness;
    });

    TArray<FBoidInfo> Result;
    int32 AliveIndex = 0;
    int32 DeadIndex = 0;
    while (AliveIndex < AliveBoids.Num() || DeadIndex < DeadBoids.Num()) {
        if (AliveIndex < AliveBoids.Num()) {
			ABoid* AliveBoid = AliveBoids[AliveIndex];

            if (DeadIndex < DeadBoids.Num()) {
                if (AliveBoid->Fitness > DeadBoids[DeadIndex].Fitness) {
					Result.Add({AliveBoid->Gene, AliveBoid->Fitness, AliveBoid->GetName()});
					AliveIndex++;
				}
                else {
                    Result.Add(DeadBoids[DeadIndex++]);
				}
            }
            else {
                Result.Add({AliveBoid->Gene, AliveBoid->Fitness, AliveBoid->GetName()});
				AliveIndex++;
            }
        }
        else if (DeadIndex < DeadBoids.Num()) {
            Result.Add(DeadBoids[DeadIndex++]);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Result[0].Name);
    PrintStengths(Result[0].Gene);
    UE_LOG(LogTemp, Warning, TEXT("Fitness: %f"), Result[0].Fitness);

    return Result;
}

UGene* UEvolutionManager::SelectParent(const TArray<FBoidInfo>& BoidData, float TotalFitness)
{
    // choose a random number between 0 and total fitness
    float Roll = FMath::RandRange(0.0f, TotalFitness);
    float CurrentFitness = 0.0f;
    for (const FBoidInfo& Ele: BoidData) {
        CurrentFitness += Ele.Fitness;
        if (CurrentFitness >= Roll)
            return Ele.Gene;
    }
    return nullptr;
}

TArray<UGene*> UEvolutionManager::BreedingMutation(const TArray<FBoidInfo>& BoidData, int32 Count, bool bIsHarvester)
{
    TArray<UGene*> Result;
    float TotalFitness = 0.0f;
    for (const FBoidInfo& Ele : BoidData)
    {
        TotalFitness += Ele.Fitness;
    }

    for (int32 i = 0; i < Count; i++) {
        // select parent here
        UGene* ParentA = SelectParent(BoidData, TotalFitness);
        UGene* ParentB = SelectParent(BoidData, TotalFitness);

        auto [ChildA, ChildB] = BreedingMutationIndividual(ParentA, ParentB, bIsHarvester);
        Result.Add(ChildA);
        Result.Add(ChildB);
    }

    return Result;
}

void UEvolutionManager::PopulationSelection(const TArray<ABoid *> &AliveBoids, int32 Count)
{
    int32 AliveBoidsSize = AliveBoids.Num();
    for (int32 i = AliveBoidsSize - 1; i > AliveBoidsSize - 1 - Count; i--) {
        AliveBoids[i]->Destroy();
    }
}
