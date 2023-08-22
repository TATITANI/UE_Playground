// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAIController.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


ABotAIController::ABotAIController()
{
}


void ABotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("ai OnPossess %s"), *InPawn->GetName());
	
	UBlackboardComponent* _Blackboard = Blackboard;
	if (UseBlackboard(BlackboardData, _Blackboard))
	{
		if (RunBehaviorTree(BehaviorTree))
		{
			
		}
	}
}

void ABotAIController::OnUnPossess()
{
	Super::OnUnPossess();

}


