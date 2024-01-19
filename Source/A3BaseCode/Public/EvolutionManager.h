// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../BoidInfo.h"

#include "EvolutionManager.generated.h"

class ABoid;

/**
 * 
 */
UCLASS()
class A3BASECODE_API UEvolutionManager : public UObject
{
	GENERATED_BODY()

public:
	UEvolutionManager();

    UGene* GenerateRandomGene(bool IsHarvester = false);

	void ApplyToBoid(UGene* Gene, ABoid* Boid, bool IsHarvester = false);

	TTuple<UGene*, UGene*> BreedingMutationIndividual(UGene* ParentA, UGene* ParentB, bool IsHarvester = false);

	void PrintStengths(UGene* Gene);

	void InitBoidGene(ABoid* Boid, bool IsHarvester = false);

    TArray<FBoidInfo> SortByFitness(TArray<ABoid*>& AliveBoids, TArray<FBoidInfo>& DeadBoids);

    UGene* SelectParent(const TArray<FBoidInfo>& BoidData, float TotalFitness);

    TArray<UGene*> BreedingMutation(const TArray<FBoidInfo>& BoidData, int32 Count, bool IsHarvester = false); // Parents selected in thie process

    void PopulationSelection(const TArray<ABoid*>& AliveBoids, int32 Count);

};


