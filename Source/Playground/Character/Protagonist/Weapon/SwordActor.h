// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "SwordActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API ASwordActor : public AWeaponActor
{
	GENERATED_BODY()

private:
	int CurrentSectionID = 1;
	const int SectionMaxID = 5;

	FName GetSectionName(int32 SectionID) const;

	UPROPERTY(EditAnywhere, Category=Attack, meta=(AllowPrivateAccess=true))
	float AttackDistance = 100.f;

	UPROPERTY(EditAnywhere, Category=Attack, meta=(AllowPrivateAccess=true))
	FVector BoxExtent = FVector(50, 50, 50);

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	TSoftObjectPtr<class UNiagaraSystem> SlashParticleSystem;
	// class UNiagaraSystem* SlashParticleSystem;
	
	
protected:
	virtual EWeaponType GetWeaponType() override { return EWeaponType::SWORD; }
	virtual ETriggerEvent GetAttackTriggerEvent() override { return ETriggerEvent::Started; };

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;


	virtual void AttackInputStarted() override;

	void CheckAttack() const;

	UFUNCTION()
	void AttackMontageEndEvent(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(EditDefaultsOnly, Category="Trail")
	class UNiagaraSystem* TrailSystem;

	UPROPERTY(EditDefaultsOnly, Category= "Trail", meta=(AllowPrivateAccess))
	FName TrailSocketTopName = "Top";

	UPROPERTY(EditDefaultsOnly, Category= "Trail", meta=(AllowPrivateAccess))
	FName TrailSocketBotName = "Bot";

	class UNiagaraComponent* TrailComponent;

public:
	virtual void Equip(AProtagonistCharacter* TargetCharacter) override;
	virtual void BeginPlay() override;
};
