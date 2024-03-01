// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Character/Bot/BotCharacter.h"
#include "BTTasK_BotChaseTo.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UBTTasK_BotChaseTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	ABotCharacter* BotOwner;

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent;
public:
	UBTTasK_BotChaseTo();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnDamaged(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
