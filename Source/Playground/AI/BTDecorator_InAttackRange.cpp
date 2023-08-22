// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_InAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Character/Bot/BotCharacter.h"

UBTDecorator_InAttackRange::UBTDecorator_InAttackRange()
{
	NodeName = TEXT("InAttackRange");
}

bool UBTDecorator_InAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ABotCharacter* OwnerBot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	ensureMsgf(OwnerBot != nullptr, TEXT("UBTDecorator_CanAttack - owner not exsit!"));

	const auto TargetProtagonist = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKeySelector.SelectedKeyName));
	ensureMsgf(TargetProtagonist != nullptr, TEXT("UBTDecorator_CanAttack - TargetProtagonist not exsit!"));

	DrawDebugSphere(OwnerBot->GetWorld(), OwnerBot->GetActorLocation(),
	                DistanceAttackable, 16, FColor::Cyan, false, 0.2f);

	const bool InAttackRange = TargetProtagonist->GetDistanceTo(OwnerBot) <= DistanceAttackable;
	return InAttackRange;
	
}
