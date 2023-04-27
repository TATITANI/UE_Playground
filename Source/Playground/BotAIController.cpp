// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAIController.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


ABotAIController::ABotAIController()
{
	UE_LOG(LogTemp, Log, TEXT("ABotAIController Constructor"));
}


void ABotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("ai OnPossess %s"), *InPawn->GetName());
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABotAIController::RandomMove, 3.f, true);
	
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

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}



void ABotAIController::RandomMove()
{
	auto CurrentPawn = GetPawn();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr)
		return;
	
	FNavLocation RandomLocation;
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 1000.f, RandomLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);
	}

	UE_LOG(LogTemp, Log, TEXT("RandomMove"));

}
