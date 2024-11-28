#include "AgentUpdateManager.h"

AAgentUpdateManager* AAgentUpdateManager::m_Instance = nullptr;

AAgentUpdateManager::AAgentUpdateManager()
{
    MaxFrameBudget = 0.01;
    TimeUsedThisFrame = 0.0;
}

AAgentUpdateManager* AAgentUpdateManager::GetInstance()
{
    if (!m_Instance)
    {
        m_Instance = NewObject<AAgentUpdateManager>();
        m_Instance->AddToRoot();
        if (GEngine && GEngine->GetWorld())
        {
            GEngine->GetWorld()->SpawnActor<AAgentUpdateManager>(AAgentUpdateManager::StaticClass());
        }
    }
    return m_Instance;
}

void AAgentUpdateManager::Destroy()
{
    if (m_Instance)
    {
        m_Instance->Reset();
        m_Instance->RemoveFromRoot();
        m_Instance = nullptr;
    }
}

void AAgentUpdateManager::Reset()
{
    TimeUsedThisFrame = 0.0;
    AgentsToUpdateNextFrame.Empty();
}

void AAgentUpdateManager::AddAgentForNextFrame(ASDTAIController* Controller)
{
    AgentsToUpdateNextFrame.AddUnique(Controller);
}


bool AAgentUpdateManager::CanUpdateAgent(ASDTAIController* Controller)
{
    
    // Si l'agent est dans la liste d'attente, il est mis à jour en priorité
    if (AgentsToUpdateNextFrame.Contains(Controller))
    {
        AgentsToUpdateNextFrame.Remove(Controller);
        return true;
    }

    // Si la liste d'attente est vide et le budget de temps n'est pas dépassé, mettre à jour l'agent
    else if (AgentsToUpdateNextFrame.IsEmpty() && TimeUsedThisFrame < MaxFrameBudget)
    {
        return true;
    }

    // Sinon, ajouter l'agent à la liste d'attente pour la prochaine frame
    else
    {
        AddAgentForNextFrame(Controller);
        return false;
    }
}


void AAgentUpdateManager::IncrementTimeUsedThisFrame(double TimeUsed)
{
    TimeUsedThisFrame += TimeUsed;
}

double AAgentUpdateManager::GetTimeUsedThisFrame()
{
    return TimeUsedThisFrame;
}

void AAgentUpdateManager::ResetTimeUsedThisFrame()
{
	TimeUsedThisFrame = 0.0;
}

void AAgentUpdateManager::DebugPrintQueue() const
{
    FString QueueContents;
    for (ASDTAIController* Controller : AgentsToUpdateNextFrame)
    {
        if (Controller)
        {
            QueueContents += Controller->GetName() + TEXT(", ");
        }
    }

    if (QueueContents.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Queue is empty"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Agents in queue: %s"), *QueueContents);
    }
}
