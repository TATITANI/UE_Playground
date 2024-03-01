// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatorlPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);


	const auto CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();

	if(ensureMsgf( (CurrentPawn != nullptr), TEXT("Could not found Bot Pawn")) == false) 
	{
		return EBTNodeResult::Failed;
	}

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if(ensureMsgf( (NavSystem != nullptr), TEXT("Could not found Bot NavSystem")) == false) 
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation RandomLocation;
	if (NavSystem->GetRandomPointInNavigableRadius(CurrentPawn->GetActorLocation(), 500.f, RandomLocation))
	{
		UE_LOG(LogTemp, Log, TEXT("actorPos %s, randomPos : %s"),
		       *CurrentPawn->GetActorLocation().ToString(), *RandomLocation.Location.ToString());

		OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolPos")), RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
