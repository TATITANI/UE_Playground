// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Data/WeaponStat.h"
#include "GunActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API AGunActor : public AWeaponActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual EWeaponType GetWeaponType() override { return EWeaponType::GUN; }
	virtual ETriggerEvent GetAttackTriggerEvent() override { return ETriggerEvent::Started; };

protected:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class AProtagonistProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay, meta=(AllowPrivateAccess = "true"))
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* FireMontage;

	virtual void AttackInputStarted() override;

private:
};
