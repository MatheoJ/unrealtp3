// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTUtils.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "DrawDebugHelpers.h"

#include "Engine/World.h"
#include <NavLinkComponent.h>


/*static*/ bool SDTUtils::Raycast(UWorld* uWorld, FVector sourcePoint, FVector targetPoint)
{
    FHitResult hitData;
    FCollisionQueryParams TraceParams(FName(TEXT("VictoreCore Trace")), true);
    
    // Fake cost for the exercise
    //Sleep(1);
    // End fake cost

    return uWorld->LineTraceSingleByChannel(hitData, sourcePoint, targetPoint, ECC_Pawn, TraceParams);
}

bool SDTUtils::IsPlayerPoweredUp(UWorld * uWorld)
{
    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(uWorld, 0);
    if (!playerCharacter)
        return false;

    ASoftDesignTrainingMainCharacter* castedPlayerCharacter = Cast<ASoftDesignTrainingMainCharacter>(playerCharacter);
    if (!castedPlayerCharacter)
        return false;

    return castedPlayerCharacter->IsPoweredUp();
}

bool SDTUtils::IsOnDeathFloor(UWorld* uWorld, FVector location, FVector& outLocation)
{
    if (!uWorld)
    {
        // Ensure the world pointer is valid
        return false;
    }
    TArray<AActor*> foundDeathFloor;
    UGameplayStatics::GetAllActorsOfClass(uWorld, AActor::StaticClass(), foundDeathFloor);
    for (AActor* actor : foundDeathFloor)
    {
        if (actor->GetName().Contains("DeathFloor"))
        {
			
            // get dimensions of the death floor
            FVector origin;
            FVector boxExtent;
            actor->GetActorBounds(false, origin, boxExtent);

            // check if the location is inside the death floor
            if (location.X > origin.X - boxExtent.X && location.X < origin.X + boxExtent.X &&
                location.Y > origin.Y - boxExtent.Y && location.Y < origin.Y + boxExtent.Y)
            {
                return true;
			}
		}
	}
    return false;
}

