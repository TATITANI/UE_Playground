// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AIController.h"
#include "BotCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/DataTable.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	NodeName = TEXT("Attack");
}


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Bot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	verify(Bot);
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

	const UBlackboardComponent* const BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComponent))
	{
		check(Bot);
		const auto TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKeySelector.SelectedKeyName));
		if (TargetActor != nullptr)
		{
			const auto dir = TargetActor->GetActorLocation() - Bot->GetActorLocation();
			auto Rot = FRotationMatrix::MakeFromX(dir).Rotator();
			Rot = FMath::RInterpTo(Bot->GetActorRotation(), Rot, DeltaSeconds, 3.0f); // 회전 보간
			Bot->SetActorRotation(Rot);
		}
	}

	if (!IsAttacking)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 태스크 종료
}
