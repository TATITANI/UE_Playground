// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_SearchTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/UtilPlayground.h"

UBTService_SearchTarget::UBTService_SearchTarget()
{
	NodeName = TEXT("SearchTarget");
	Interval = 1.0f;
}

void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();
	ensure(CurrentPawn != nullptr);

	const UWorld* World = CurrentPawn->GetWorld();
	ensure(World != nullptr);

	FHitResult HitResult;
	const bool bOverlap = UKismetSystemLibrary::SphereTraceSingle(World, CurrentPawn->GetActorLocation(), CurrentPawn->GetActorLocation(),
	                                                              SearchRadius, TraceTypeQuery, false, {},
	                                                              EDrawDebugTrace::None, HitResult, true,
	                                                              FLinearColor::Green, FLinearColor::Red,  0.5f);
	if (bOverlap)
	{
		const auto TargetActor = HitResult.GetActor();
		UE_LOG(LogTemp,Log,TEXT("SearchTarget Name :%s"), *TargetActor->GetName());
		if (TargetActor->IsA(TargetClass))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKeySelector.SelectedKeyName, TargetActor);
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKeySelector.SelectedKeyName, nullptr);
	}

}
