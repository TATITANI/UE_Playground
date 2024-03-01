// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasK_BotChaseTo.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Utils/UtilPlayground.h"

UBTTasK_BotChaseTo::UBTTasK_BotChaseTo()
{
	NodeName = TEXT("ChaseTo");
}

EBTNodeResult::Type UBTTasK_BotChaseTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BotOwner = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	if (ensure(BotOwner != nullptr) == false)
		return EBTNodeResult::Failed;

	BehaviorTreeComponent = &OwnerComp;
	BotOwner->OnTakeAnyDamage.AddUniqueDynamic(this, &UBTTasK_BotChaseTo::OnDamaged);

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTasK_BotChaseTo::OnDamaged(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	BehaviorTreeComponent->GetAIOwner()->StopMovement();
	FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Aborted);
}
