// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MyBTService_find_collectible.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UMyBTService_find_collectible : public UBTService
{
	GENERATED_BODY()
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
