// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AIController.h"
#include "BotCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	NodeName = TEXT("Attack");
}


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Bot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Bot == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	AttackEndHandle = Bot->OnAttackEnd->AddLambda([this]()
	{
		IsAttacking = false;
		Bot->OnAttackEnd->Remove(AttackEndHandle);
		UE_LOG(LogTemp, Log, TEXT("OnAttackEnd"));
	});

	IsAttacking = true;
	Bot->Attack();

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 태스크 종료
}
