// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Gene.generated.h"

UENUM()
enum class EGeneKey : uint8 {
	VelocityMatching,
	AvoidBoids,
	Centering,
	ObstacleAvoidance,
	AsteroidCluster,
	Speed,
	EnumSize
};

/**
 * 
 */
UCLASS()
class A3BASECODE_API UGene : public UObject
{
	GENERATED_BODY()

public:
	UGene() = default;

	float Genes[(uint8)EGeneKey::EnumSize] = {};
};
