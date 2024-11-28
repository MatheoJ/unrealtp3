// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToLocation.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "SDTAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GroupManager.h"


EBTNodeResult::Type UBTTask_MoveToLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if (ASDTAIController* controller = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		if (controller->AtJumpSegment || controller->InAir)
			return EBTNodeResult::Failed;

		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass()) {

			UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
			if (BlackboardComp != nullptr) {
				FVector destination = BlackboardComp->GetValueAsVector(GetSelectedBlackboardKey());

				controller->MoveToLocation(destination, 0.5f, false, true, false, NULL, false);

				return EBTNodeResult::Succeeded;
			} else {
				UE_LOG(LogTemp, Warning, TEXT("There was no blackboard"));
			}
		}
	}
	return EBTNodeResult::Failed;	
}
