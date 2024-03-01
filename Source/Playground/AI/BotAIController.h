// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BotAIController.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API ABotAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABotAIController();
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	void ActiveBehaviorTree(bool IsActive);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=AI, meta=(AllowPrivateAccess))
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AI, meta=(AllowPrivateAccess))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AI, meta=(AllowPrivateAccess))
	class UBlackboardData* BlackboardData;

	
};
