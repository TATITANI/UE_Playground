// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SearchTarget.generated.h"
/**
 * 
 */
UCLASS()
class PLAYGROUND_API UBTService_SearchTarget : public UBTService
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta=(ClampMin="0.1", AllowPrivateAccess=true))
	float SearchRadius = 500.f;


	UPROPERTY(EditInstanceOnly, Category=BlackBoard, meta=(AllowPrivateAccess=true))
	FBlackboardKeySelector TargetKeySelector;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<class ACharacter> TargetClass;
	
public:
	UBTService_SearchTarget();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


};
