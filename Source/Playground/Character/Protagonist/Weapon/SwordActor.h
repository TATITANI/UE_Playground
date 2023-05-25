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
	/**
	 * @brief range : 1~6
	 */
	int SectionID = 1;
	const int SectionMaxID = 6;

	FName GetSectionName() const;
	
	UPROPERTY(EditAnywhere, Category=Attack, meta=(AllowPrivateAccess=true))
	float AttackDistance = 100.f;

	UPROPERTY(EditAnywhere, Category=Attack, meta=(AllowPrivateAccess=true))
	FVector BoxExtent = FVector(50,50,50);
	
protected:
	virtual void BindInputActions(UEnhancedInputComponent* EnhancedInputComponent) override;
	virtual EWeaponType GetWeaponType() override { return SWORD; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* AttackInputAction;

	UFUNCTION(meta=(AllowPrivateAccess = "true"))
	void Attack();

	void AttackCheck() const;

	UFUNCTION()
	void AttackEndEvent(UAnimMontage* Montage, bool bInterrupted);	
};
