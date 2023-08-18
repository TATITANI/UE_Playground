// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BotAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnAttackHit);
DECLARE_MULTICAST_DELEGATE(FOnAttackEnded);

UCLASS()
class PLAYGROUND_API UBotAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UBotAnimInstance();

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess = "true", BlueprintThreadSafe))
	FVector CurrentVelocity = FVector::ZeroVector;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	UAnimMontage* AttackedMontage;
	
	// "AnimNotify_노티파이명(AttackHit)" 네이밍 필요
	UFUNCTION()
	void AnimNotify_AttackHit(); 

public:
	FOnAttackHit OnAttackHit;
	TSharedPtr<FOnAttackEnded> OnAttackEnded;

public:
	void PlayAttackMontage();
	void PlayAttackedMontage();

	UFUNCTION()
	void MontageEnd(UAnimMontage* Montage, bool bInterrupted);
};
