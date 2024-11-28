// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingGameMode.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTrainingCharacter.h"
#include "AgentUpdateManager.h"

ASoftDesignTrainingGameMode::ASoftDesignTrainingGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASoftDesignTrainingPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_SDTMainCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
}

void ASoftDesignTrainingGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AAgentUpdateManager* UpdateManager = AAgentUpdateManager::GetInstance();
	if (UpdateManager)
		{
			UpdateManager->ResetTimeUsedThisFrame();
		}
}



void ASoftDesignTrainingGameMode::StartPlay()
{
	Super::StartPlay();

	// Destroy the existing instance of AgentUpdateManager if it exists
	AAgentUpdateManager* ExistingInstance = AAgentUpdateManager::GetInstance();
	if (ExistingInstance)
	{
		ExistingInstance->Destroy();
	}

	// Create a new instance for the new simulation
	AAgentUpdateManager* NewInstance = AAgentUpdateManager::GetInstance();
	NewInstance->Reset();

	GetWorld()->Exec(GetWorld(), TEXT("stat fps"));
}