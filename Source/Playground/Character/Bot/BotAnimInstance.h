// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotCharacter.h"
#include "Animation/AnimInstance.h"
#include "BotAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnAttackHit);

UCLASS()
class PLAYGROUND_API UBotAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UBotAnimInstance();

public:
	virtual void NativeBeginPlay() override;

	// "AnimNotify_노티파이명(AttackHit)" 네이밍 필요
	UFUNCTION()
	void AnimNotify_AttackHit();

	UFUNCTION()
	void AnimNotify_StandUp();
	
	void PlayBotMontage(UAnimMontage* Montage, EBotState::Type ResetState);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	UAnimMontage* AttackedMontage;
	
	class ABotCharacter* OwnerBot;

	TMap<UAnimMontage*, EBotState::Type> BotStateByMontage;
public:
	FOnAttackHit OnAttackHit;

public:
	void PlayAttackMontage(EBotState::Type ResetState = EBotState::Idle);
	void PlayAttackedMontage(EBotState::Type ResetState = EBotState::Idle);
	bool IsPlayingAttackedMontage() const { return Montage_IsPlaying(AttackedMontage); }

	UFUNCTION()
	void MontageEnd(UAnimMontage* Montage, bool bInterrupted);
};
