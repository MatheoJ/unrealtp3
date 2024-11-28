// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_find_collectible.h"

#include "SDTCollectible.h"
#include "MyBTService_find_flee_location.h"
#include "SoftDesignTraining/SDTUtils.h"
#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingCharacter.h"
#include "SDTFleeLocation.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AgentUpdateManager.h"

void UMyBTService_find_collectible::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	double startTime = FPlatformTime::Seconds();

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	ASDTAIController* AIController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
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
	if (!selfPawn)
	{
		return;
	}

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
			FString debugMessage = FString::Printf(TEXT("Find collectible time : %f ms"), timeTakenInMilliseconds);

			if (APawn* selfPawn = AIController->GetPawn())
			{
				FVector debugLocationForFleeTime = selfPawn->GetActorLocation() + FVector(-20, 0, 100);
				DrawDebugString(GetWorld(), debugLocationForFleeTime, debugMessage, nullptr, FColor::Green, DeltaSeconds, true);
			}
		};

	FVector collectibleLocation = FVector::ZeroVector;
	const float startingOffset = 100;
	const float capsuleHalfLenght = 500;
	const float capsuleRadius = 250.f;
	FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * startingOffset;
	FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * capsuleHalfLenght * 2;

	TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
	detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_COLLECTIBLE));

	TArray<FHitResult> allDetectionHits;
	GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(capsuleRadius));

	ASDTCollectible* collectibleActor = nullptr;
	for (FHitResult &detectionHit : allDetectionHits)
	{
		// if we detect a collectible
		if (detectionHit.GetActor()->IsA(ASDTCollectible::StaticClass()))
		{
			// if the collectible is not on cooldown
			collectibleActor = Cast<ASDTCollectible>(detectionHit.GetActor());
			if (!collectibleActor->IsOnCooldown())
			{
				collectibleLocation = detectionHit.GetActor()->GetActorLocation();
			}
			break;
		}
	}

	if (collectibleLocation == FVector::ZeroVector)
	{
		if(BlackboardComp == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Blackboard is null"));
			return;
		}

		if (ASDTCollectible* collectibleTarget = Cast<ASDTCollectible>(BlackboardComp->GetValueAsObject("CollectibleTarget"))) {
			if (collectibleTarget != nullptr && !collectibleTarget->IsOnCooldown()) {
				measureAndLogTime();
				return;
			}
		}

		TArray<AActor*> foundCollectibles;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

		while (foundCollectibles.Num() != 0)
		{
			int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

			collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
			if (!collectibleActor)
				return;

			if (!collectibleActor->IsOnCooldown())
			{
				collectibleLocation = collectibleActor->GetActorLocation();
				measureAndLogTime();
				break;
			}
			else
			{
				foundCollectibles.RemoveAt(index);
			}
		}
	}
	// Draw debug sphere on the collectible target
	FColor capsuleColor = collectibleLocation != FVector::ZeroVector ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), collectibleLocation, 75, 8, capsuleColor, false, 0.5f);
	// Update the Blackboard
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector("CollectibleLocation", collectibleLocation);
		BlackboardComp->SetValueAsObject("CollectibleTarget", collectibleActor);
	}

	measureAndLogTime();
}
