// Fill out your copyright notice in the Description page of Project Settings.

#include "GroupManager.h"
#define RADIUS 500.f

UGroupManager *UGroupManager::Instance = nullptr;

UGroupManager::UGroupManager()
{
	Agents = TArray<AActor *>();
	CirclingPositions = TArray<FVector>();
	LKP.position = FVector::ZeroVector;
	LKP.valid = false;
}

UGroupManager::~UGroupManager()
{
	Agents.Empty();
}

UGroupManager *UGroupManager::GetInstance()
{
	if (!Instance)
	{
		Instance = NewObject<UGroupManager>();
	}
	return Instance;
}

void UGroupManager::AddAgent(AActor *Agent)
{
	if (Agent != nullptr && !isInGroup(Agent))
	{
		Agents.Add(Agent);
		if (LKPisValid())
			GenerateCirclingPositions();
	}
}

void UGroupManager::RemoveAgent(AActor *Agent)
{
	if (Agent != nullptr && isInGroup(Agent))
	{
		Agents.Remove(Agent);
	}
}

TArray<AActor *> UGroupManager::getAgents()
{
	return Agents;
}

void UGroupManager::ClearGroup()
{
	Agents.Empty();
}

bool UGroupManager::isInGroup(AActor *Agent)
{
	if (Agent && Agent != nullptr)
	{
		return Agents.Contains(Agent);
	}
	return false;
}

void UGroupManager::SetLKP(FVector _LKP)
{
	LKP.position = _LKP;
	LKP.valid = true;
}

UGroupManager::LKPStruct UGroupManager::getLKP()
{
	return LKP;
}

void UGroupManager::invalidateLKP()
{
	LKP.valid = false;
}

bool UGroupManager::LKPisValid()
{
	return LKP.valid;
}

TArray<FVector> UGroupManager::GeneratePointsAroundCircle(FVector center, float radius, int nbPoints)
{
	TArray<FVector> points = TArray<FVector>();

	float angle = 0.f;
	float angleIncrement = 2 * PI / nbPoints;
	for (int i = 0; i < nbPoints; i++)
	{
		float x = center.X + radius * cos(angle);
		float y = center.Y + radius * sin(angle);
		points.Add(FVector(x, y, center.Z));
		angle += angleIncrement;
	}
	return points;
}

void UGroupManager::GenerateCirclingPositions()
{
	CirclingPositions = GeneratePointsAroundCircle(LKP.position, RADIUS, Agents.Num());
}

FVector UGroupManager::GetCirclingPosition(AActor *Agent)
{
	if (Agent != nullptr && isInGroup(Agent))
	{
		// This ensures that the circling positions are generated
		if (CirclingPositions.Num() == 0)
		{
			return FVector::ZeroVector;
		}
		int index = Agents.Find(Agent);
		if (index != INDEX_NONE)
		{
			FVector position = CirclingPositions[index];

			// Chase the player directly
			if (FVector::Dist(Agent->GetActorLocation(), LKP.position) < RADIUS + 50.0f)
			{
				return LKP.position;
			}
			return CirclingPositions[index];
		}
	}
	return FVector::ZeroVector;
}
