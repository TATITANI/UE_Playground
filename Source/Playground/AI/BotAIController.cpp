// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Utils/UtilPlayground.h"

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

	BehaviorTreeComponent->StartLogic();
	RunBehaviorTree(BehaviorTreeComponent->GetCurrentTree());

}

void ABotAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ABotAIController::StartBehaviorTreeLogic()
{
	BehaviorTreeComponent->StartLogic();
	
}

void ABotAIController::StopBehaviorTree(FString Reason)
{
	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}
