// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_IsAttacked.h"

#include "AIController.h"
#include "Character/Bot/BotCharacter.h"

UBTDecorator_IsAttacked::UBTDecorator_IsAttacked()
{
	
}

bool UBTDecorator_IsAttacked::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ABotCharacter* OwnerBot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	ensureMsgf(OwnerBot != nullptr, TEXT("UBTDecorator_CanAttack - owner not exsit!"));

	return  OwnerBot->IsAttacked();

}
