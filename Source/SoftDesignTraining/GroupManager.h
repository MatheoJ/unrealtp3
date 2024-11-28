// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GroupManager.generated.h"

/**
 *
 */
UCLASS()
class SOFTDESIGNTRAINING_API UGroupManager : public UObject
{
	GENERATED_BODY()

public:
	static UGroupManager *GetInstance();

	void AddAgent(AActor *Agent);
	void RemoveAgent(AActor *Agent);
	void ClearGroup();
	TArray<AActor *> getAgents();

	bool isInGroup(AActor *Agent);

	struct LKPStruct
	{
		FVector position;
		bool valid;
	};
	LKPStruct LKP;

	void SetLKP(FVector LKP);
	LKPStruct getLKP();
	void invalidateLKP();
	bool LKPisValid();

	void GenerateCirclingPositions();
	FVector GetCirclingPosition(AActor *Agent);

private:
	UGroupManager();
	~UGroupManager();
	static UGroupManager *Instance;

	TArray<FVector> GeneratePointsAroundCircle(FVector center, float radius, int nbPoints);
	UPROPERTY()
	TArray<AActor *> Agents;
	UPROPERTY()
	TArray<FVector> CirclingPositions;
};
