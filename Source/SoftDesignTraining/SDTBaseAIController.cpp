// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTBaseAIController.h"
#include "SoftDesignTraining.h"
#include "GroupManager.h"


ASDTBaseAIController::ASDTBaseAIController(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    m_ReachedTarget = true;
}

void ASDTBaseAIController::Tick(float deltaTime)
{
	UGroupManager *groupManager = UGroupManager::GetInstance();
	AActor* actor = GetPawn();
	if (groupManager && actor)
	{
		if (groupManager->isInGroup(actor))
		{
			DrawDebugSphere(GetWorld(), actor->GetActorLocation() + FVector::UpVector * 8, 30.f, 30.f, FColor::Cyan);
		}
	}

}

void ASDTBaseAIController::OnPossess(class APawn* InPawn)
{
	Super::OnPossess(InPawn);
    if (m_BehaviorTree != nullptr) {
        UBlackboardComponent* b;
        UseBlackboard(m_BehaviorTree->BlackboardAsset, b);
        Blackboard = b;
        RunBehaviorTree(m_BehaviorTree);
    }
}



