// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_SearchTarget.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SearchTarget::UBTService_SearchTarget()
{
	NodeName = TEXT("SearchTarget");
	Interval = 1.0f;
}

void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	const auto CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	if (CurrentPawn == nullptr)
		return;
	
	const UWorld* World = CurrentPawn->GetWorld();
	const FVector Center = CurrentPawn->GetActorLocation();
	
	if (World == nullptr)
		return;
	
	TArray<FOverlapResult> OverlapResults;
	
	const bool IsOverlap = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_GameTraceChannel3, // "attack"채널. config/DefaultEngine.ini 에서 확인 가능
		FCollisionShape::MakeSphere(SearchRadius),
		FCollisionQueryParams(NAME_None, false, CurrentPawn)
	);
	
	if (IsOverlap)
	{
		for (auto& OverlapResult : OverlapResults)
		{
			auto TargetActor = OverlapResult.GetActor();
			bool IsMatch = TargetActor->IsA(TargetClass);
			if (IsMatch)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKeySelector.SelectedKeyName, TargetActor);
				DrawDebugSphere(World, Center, SearchRadius, 16, FColor::Green, false, 0.2f);
				return;
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKeySelector.SelectedKeyName, nullptr);
		DrawDebugSphere(World, Center, SearchRadius, 16, FColor::Red, false, 0.2f);

	}

}
