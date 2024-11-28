// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_updateShouldTickBT.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AgentUpdateManager.h"
#include "SDTAIController.h"

void UMyBTService_updateShouldTickBT::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    ASDTAIController* Controller = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (!Controller)
    {
        return;
    }

    AAgentUpdateManager* UpdateManager = AAgentUpdateManager::GetInstance();

    if (UpdateManager && UpdateManager->CanUpdateAgent(Controller))
    {
        BlackboardComp->SetValueAsBool("ShouldTickBT", true);
    }
    else
    {
        BlackboardComp->SetValueAsBool("ShouldTickBT", false);
    }
}