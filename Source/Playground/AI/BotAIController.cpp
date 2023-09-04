// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"

ABotAIController::ABotAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
}

void ABotAIController::BeginPlay()
{
	Super::BeginPlay();
}


void ABotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// UE_LOG(LogTemp, Log, TEXT("ai OnPossess %s"), *InPawn->GetName());

	UBlackboardComponent* _Blackboard = Blackboard;
	if (UseBlackboard(BlackboardData, _Blackboard))
	{
	}
}

void ABotAIController::OnUnPossess()
{
	Super::OnUnPossess();

}

void ABotAIController::ActiveBehaviorTree(bool IsActive)
{
	if(IsActive)
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	else
		BehaviorTreeComponent->StopTree();	
}
