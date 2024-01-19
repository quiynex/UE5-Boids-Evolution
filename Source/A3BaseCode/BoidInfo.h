#pragma once

#include "CoreMinimal.h"
#include "BoidInfo.generated.h"

class UGene;

USTRUCT()
struct FBoidInfo {
	GENERATED_BODY()

	UPROPERTY() UGene* Gene;
	float Fitness;
	FString Name;
};
