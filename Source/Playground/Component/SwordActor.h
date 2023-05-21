// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/WeaponActor.h"
#include "SwordActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API ASwordActor : public AWeaponActor
{
	GENERATED_BODY()

public:

protected:
	virtual void BindInputActions(UEnhancedInputComponent* EnhancedInputComponent) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* AttackInputAction;

	UFUNCTION(meta=(AllowPrivateAccess = "true"))
	void Attack();
};
