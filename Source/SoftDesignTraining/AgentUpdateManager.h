#pragma once

#include "CoreMinimal.h"
#include "SDTAIController.h"
#include "AgentUpdateManager.generated.h"

UCLASS()
class AAgentUpdateManager : public AActor
{
    GENERATED_BODY()

public:
    static AAgentUpdateManager* GetInstance();
    static void Destroy();

    void AddAgentForNextFrame(ASDTAIController* Controller);
    bool CanUpdateAgent(ASDTAIController* Controller);
    void IncrementTimeUsedThisFrame(double TimeUsed);
    double GetTimeUsedThisFrame();
    void ResetTimeUsedThisFrame();
    void Reset();
    void DebugPrintQueue() const;

private:
    AAgentUpdateManager();
    static AAgentUpdateManager* m_Instance;

    UPROPERTY()
    TArray<ASDTAIController*> AgentsToUpdateNextFrame;
    double MaxFrameBudget;
    double TimeUsedThisFrame;
};