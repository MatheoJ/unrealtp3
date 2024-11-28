// Fill out your copyright notice in the Description page of Project Settings.


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

void UMyBTService_find_flee_location::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    double startTime = FPlatformTime::Seconds();

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
    APawn* selfPawn = AIController->GetPawn();
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
            FString debugMessage = FString::Printf(TEXT("Flee location time : %f ms"), timeTakenInMilliseconds);

            if (APawn* selfPawn = AIController->GetPawn())
            {
                FVector debugLocationForFleeTime = selfPawn->GetActorLocation() + FVector(-60, 0, 100);
                DrawDebugString(GetWorld(), debugLocationForFleeTime, debugMessage, nullptr, FColor::Red, DeltaSeconds, true);
            }
        };

    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;


    for (TActorIterator<AActor> actorIterator(GetWorld()); actorIterator; ++actorIterator)
    {
        ASDTFleeLocation* fleeLocation = Cast<ASDTFleeLocation>(*actorIterator);
        if (fleeLocation)
        {
            float distToFleeLocation = FVector::Dist(fleeLocation->GetActorLocation(), playerCharacter->GetActorLocation());

            FVector selfToPlayer = playerCharacter->GetActorLocation() - selfPawn->GetActorLocation();
            selfToPlayer.Normalize();

            FVector selfToFleeLocation = fleeLocation->GetActorLocation() - selfPawn->GetActorLocation();
            selfToFleeLocation.Normalize();

            float fleeLocationToPlayerAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(selfToPlayer, selfToFleeLocation)));
            float locationScore = distToFleeLocation + fleeLocationToPlayerAngle * 100.f;

            if (locationScore > bestLocationScore)
            {
                bestLocationScore = locationScore;
                bestFleeLocation = fleeLocation;
            }
            //DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
        }
    }
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector("FleeLocation", bestFleeLocation->GetActorLocation());
    }

    measureAndLogTime();
    return;
}