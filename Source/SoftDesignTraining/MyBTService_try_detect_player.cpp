// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_try_detect_player.h"
#include "SoftDesignTraining/SDTUtils.h"
#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingCharacter.h"
#include "GroupManager.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "SDTUtils.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"
#include "AgentUpdateManager.h"



void UMyBTService_try_detect_player::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    ASDTAIController* AIController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());

    double startTime = FPlatformTime::Seconds();


    if (!AIController)
    {
        return;
    }

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter)
    {
        return;
    }
    APawn *selfPawn = AIController->GetPawn();

    auto measureAndLogTime = [&]()
        {
            double endTime = FPlatformTime::Seconds();
            double timeTakenInSeconds = endTime - startTime;
            double timeTakenInMilliseconds = timeTakenInSeconds * 1000.0;
            AAgentUpdateManager* UpdateManager = AAgentUpdateManager::GetInstance();
            if (UpdateManager)
            {
                UpdateManager->IncrementTimeUsedThisFrame(timeTakenInSeconds);
            }
            FString debugMessage = FString::Printf(TEXT("Player detection time : %f ms"), timeTakenInMilliseconds);

            if (APawn* selfPawn = AIController->GetPawn())
            {
                FVector debugLocation = selfPawn->GetActorLocation() + FVector(20, 0, 100);
                DrawDebugString(GetWorld(), debugLocation, debugMessage, nullptr, FColor::Yellow, DeltaSeconds, true);
            }
        };
    
    if (!selfPawn)
    {
        measureAndLogTime();
		return;
	}
	UGroupManager *groupManager = UGroupManager::GetInstance();
	if (!groupManager)
	{
		return;
	}

    bool bHasLineOfSight = false;
    const float startingOffset = 100;
    const float capsuleHalfLenght = 500;
    const float capsuleRadius = 250.f;
    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * startingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * capsuleHalfLenght * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(capsuleRadius));

    for (FHitResult &hit : allDetectionHits)
    {

        if (UPrimitiveComponent* component = hit.GetComponent())
        {
            if (component->GetCollisionObjectType() == COLLISION_PLAYER)
            {
                bHasLineOfSight = true;
                break;
            }
        }
	}
    // draw debug capsule
    FColor capsuleColor = bHasLineOfSight ? FColor::Green : FColor::Red;
    DrawDebugCapsule(GetWorld(), detectionStartLocation + capsuleHalfLenght * selfPawn->GetActorForwardVector(), capsuleHalfLenght, capsuleRadius, 
                    selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), capsuleColor, false, 0.5f);

    // UpdatePlayerInteractionBehavior(detectionHit, deltaTime);
    // Update the Blackboard
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    
    FVector outLocation;
    SDTUtils::IsOnDeathFloor(GetWorld(), PlayerCharacter->GetActorLocation(), outLocation);
	
	// Player is seen
    if (BlackboardComp && bHasLineOfSight)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        if (SDTUtils::IsOnDeathFloor(GetWorld(), PlayerLocation, outLocation)) {
            measureAndLogTime();
            return;
        }
		groupManager->AddAgent(selfPawn);
		groupManager->SetLKP(PlayerLocation);
		FVector LKP = groupManager->getLKP().position;

		groupManager->GenerateCirclingPositions();
		FVector chasingPosition = groupManager->GetCirclingPosition(selfPawn);

        BlackboardComp->SetValueAsVector("PlayerLKLocation", chasingPosition);
        BlackboardComp->SetValueAsBool("LKPLayerLocationValid", true);
        BlackboardComp->SetValueAsBool("IsPlayerPoweredUp", SDTUtils::IsPlayerPoweredUp(GetWorld()));

        DrawDebugSphere(GetWorld(), chasingPosition, 50.f, 16, chasingPosition == FVector::ZeroVector ? FColor::Red : FColor::Blue, false, 1.f);
        measureAndLogTime();
        return;
    }
    
	// Player is not seen but LKP is valid
    if (groupManager->isInGroup(selfPawn) && groupManager->LKPisValid()) {
		FVector LKP = groupManager->getLKP().position;
		FVector chasingPosition = groupManager->GetCirclingPosition(selfPawn);
		// if he is not on top of the LKP position
		if (FVector::Dist(LKP, selfPawn->GetActorLocation()) > 50.f)
		{
			BlackboardComp->SetValueAsVector("PlayerLKLocation", chasingPosition);
			BlackboardComp->SetValueAsBool("LKPLayerLocationValid", true);
			BlackboardComp->SetValueAsBool("IsPlayerPoweredUp", false);
            
			DrawDebugSphere(GetWorld(), chasingPosition, 50.f, 16, chasingPosition == FVector::ZeroVector ? FColor::Green : FColor::Purple, false, 1.f);
			measureAndLogTime();
			return;
		}
    }


	// Player is not seen and LKP is invalid
	if (groupManager->isInGroup(selfPawn))
	{
		groupManager->RemoveAgent(selfPawn);
		// invalidate all the blackboard values for all the agents in the group
		if (groupManager->getLKP().valid) {
			groupManager->invalidateLKP();
			for (AActor* agent : groupManager->getAgents())
			{
                if(!agent) {
                    UE_LOG(LogTemp, Warning, TEXT("Agent undefined - try_detect"));
                    continue;
                }
				if (ASDTAIController* controller = Cast<ASDTAIController>(agent->GetInstigatorController()))
				{
					UBlackboardComponent* blackboard = controller->GetBlackboardComponent();
					if (blackboard)
					{
						blackboard->SetValueAsBool("LKPLayerLocationValid", false);
						blackboard->SetValueAsVector("PlayerLKLocation", FVector::ZeroVector);
						blackboard->SetValueAsBool("IsPlayerPoweredUp", false);
					}
				}
			}
		}
	}
	BlackboardComp->SetValueAsBool("LKPLayerLocationValid", false);
	BlackboardComp->SetValueAsVector("PlayerLKLocation", FVector::ZeroVector);
	BlackboardComp->SetValueAsBool("IsPlayerPoweredUp", false);
	measureAndLogTime();
	return;
}