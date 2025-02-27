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
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess))
	FVector LowerAttackLaunchVelocity = FVector(0,0,-2000);
	
	UPROPERTY(Replicated)
	uint8 CurrentAttackSectionID = 0;

	const uint8 DefaultAttackSectionMaxID = 3;

	UPROPERTY()
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

	// 서버에서 관리
	UPROPERTY()
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
	class ALevelSequenceActor* SequenceActor;

	UPROPERTY()
	class UNiagaraComponent* TrailComponent;

	UPROPERTY(Replicated)
	UNiagaraComponent* LowerAttackNiagaraComponent;

	TMap<UAnimMontage*, FSimpleDelegate> AttackMontageEndEventMap;

protected:
	virtual void BindInputActionsImpl(UEnhancedInputComponent* EnhancedInputComponent) override final;
	virtual EWeaponType GetWeaponType() override { return EWeaponType::SWORD; }
	virtual ETriggerEvent GetAttackTriggerEvent() override { return ETriggerEvent::Started; };

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<Sword::EAttackType> CurrentAttackType = Sword::EAttackType::Default;

	UFUNCTION()
	void SetCurrentAttackType(Sword::EAttackType AttackType);
	
	UFUNCTION(Server, Reliable)
	void ServerSetCurrentAttackType(Sword::EAttackType AttackType);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	virtual void AttackInputStarted() override;

	void InitTrail();
	
	UFUNCTION()
	void PlayMontage(UAnimMontage* Montage, FName SectionName = FName());

	void ApplyDamageBot(ABotCharacter* Bot, bool bKnockOut = false) const;


	/********* Ground Attack ********/
	void AttackOnGround();

	TArray<FHitResult> TraceGroundAttack(const FTransform& TrfProtagonist);
	
	void GroundAttackToBots(const FTransform& TrfProtagonist, const TArray<FHitResult> &HitResults);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttackOnGroundEvent(float AttackTimeGap, FTransform TrfProtagonist);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackOnGroundEvent(int32 MontageSectionID, const TArray<FVector_NetQuantize100> &HitPoints);


	/********* Upper Attack ********/
	UFUNCTION()
	void TriggerUpperAttack();

	UFUNCTION(Server, Reliable)
	void ServerTriggerUpperAttackEvent();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTriggerUpperAttack();

	UFUNCTION()
	void UpperAttackCombo();

	UFUNCTION(Server, Reliable)
	void ServerUpperAttackComboEvent(int ComboNum);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpperAttackComboEvent(int ComboNum,const TArray<int32>& BotNetObjectIDs);
	
	UFUNCTION()
	void JumpUpperAttackMontageEndEvent();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpUpperFullAttackEvent();


	/********* Lower Attack ********/
	void LowerAttack();

	TArray<FHitResult> TraceLowerAttack(const FTransform& TrfProtagonist);

	UFUNCTION(Server, Reliable)
	void ServerLowerAttackEvent(FTransform TrfProtagonist);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLowerAttackEvent();
	
	FName GetGroundAttackSectionName(int32 SectionID) const;

	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void GroundAttackMontageEndEvent();


	
private:	
	UFUNCTION()
	void OnChangedProtagonistMovementMode(ACharacter* Character, EMovementMode PrevMovementMode,
	                                      uint8 PreviousCustomMode);

	void SpawnLowerAttackLandingFX();

	UFUNCTION()
	void SetActiveTrail(bool bActive);


	virtual void Tick(float DeltaSeconds) override;

public:
	ASwordActor();
	virtual void Equip(AProtagonistCharacter* TargetCharacter) override;
	virtual void UnEquip() override;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
};
