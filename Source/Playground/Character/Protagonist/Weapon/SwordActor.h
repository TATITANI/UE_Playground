// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"

#include "SwordActor.generated.h"


UENUM(BlueprintType)
namespace Sword
{
	enum EAttackType
	{
		None,
		Default,
		Upper,
		Lower,
	};
}

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API ASwordActor : public AWeaponActor
{
	GENERATED_BODY()

private:
	uint8 CurrentAttackSectionID = 0;
	const uint8 DefaultAttackSectionMaxID = 3;

	uint8 CurrentUpperComboNum = 1;

	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* UpperAttackTriggerInputAction;

	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LowerAttackTriggerInputAction;

	UPROPERTY(EditDefaultsOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	uint8 UpperAttackComboMaxCnt = 3;

	UPROPERTY(EditAnywhere, Category=Attack, meta=(AllowPrivateAccess=true))
	FVector AttackRange = FVector(150, 200, 50);

	UPROPERTY(EditDefaultsOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	TEnumAsByte<ETraceTypeQuery> AttackTraceType;

	TArray<class ABotCharacter*> LastGroundHitBots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* DefaultAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* UpperAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* LowerAttackMontage;

	double LastAttackTime = 0;

	UPROPERTY(EditDefaultsOnly, Category=Effect, meta=(AllowPrivateAccess))
	TSoftObjectPtr<class UNiagaraSystem> SlashParticleSystem;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	class UNiagaraSystem* LowerAttackFX;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	class UNiagaraSystem* LowerLandingFX;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	class UNiagaraSystem* UpperLastShotFX;

	UPROPERTY(EditDefaultsOnly, Category= Effect, meta=(AllowPrivateAccess))
	FName TrailSocketTopName = "Top";

	UPROPERTY(EditDefaultsOnly, Category= Effect, meta=(AllowPrivateAccess))
	FName TrailSocketBotName = "Bot";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EObjectTypeQuery> GroundObjectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ULevelSequence* UpperAttackSequence;

	class ULevelSequencePlayer* UpperAttackSequencePlayer;

	class UNiagaraComponent* TrailComponent;
	UNiagaraComponent* LowerAttackNiagaraComponent;

	TMap<UAnimMontage*, FSimpleDelegate> AttackMontageEndEventMap;

protected:
	virtual EWeaponType GetWeaponType() override { return EWeaponType::SWORD; }
	virtual ETriggerEvent GetAttackTriggerEvent() override { return ETriggerEvent::Started; };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<Sword::EAttackType> CurrentAttackType = Sword::EAttackType::Default;

private:
	virtual void AttackInputStarted() override;
	void GroundAttackToBots();
	void AttackOnGround();

	void TriggerUpperAttack();
	void UpperAttackCombo();

	void LowerAttack();

	void AttackToBot(ABotCharacter* Bot, TOptional<FVector> HitPoint, bool bKnockOut = false);

	FName GetGroundAttackSectionName(int32 SectionID) const;

	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void GroundAttackMontageEndEvent();

	UFUNCTION()
	void JumpUpperAttackMontageEndEvent();

	UFUNCTION()
	void OnChangedProtagonistMovementMode(ACharacter* Character, EMovementMode PrevMovementMode,
	                                      uint8 PreviousCustomMode);

	void SpawnLowerAttackLandingFX();

public:
	virtual void Equip(AProtagonistCharacter* TargetCharacter) override;
	virtual void UnEquip() override;
	virtual void BeginPlay() override;
};
