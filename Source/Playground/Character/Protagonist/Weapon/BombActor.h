// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Data/WeaponStat.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "BombActor.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API ABombActor : public AWeaponActor
{
	GENERATED_BODY()

public:
	ABombActor();

private:
	UPROPERTY(EditDefaultsOnly, Category=Weapon, meta=(AllowPrivateAccess=true))
	TSubclassOf<class AFlyingBomb> FlyingBombClass;

	UPROPERTY(EditDefaultsOnly, Category=Ani, meta=(AllowPrivateAccess=true))
	UAnimMontage* ThrowMontage;

private: // 궤적
	UPROPERTY(VisibleAnywhere, Category=Trajectory, meta=(AllowPrivateAccess=true))
	TArray<class USplineMeshComponent*> SplineMeshes;

	UPROPERTY(EditDefaultsOnly, Category=Trajectory, meta=(AllowPrivateAccess=true))
	class USplineComponent* SplineComponent;

	UPROPERTY(EditDefaultsOnly, Category=Trajectory, meta=(AllowPrivateAccess=true))
	class UStaticMesh* SplineStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category=Trajectory, meta=(AllowPrivateAccess=true))
	FPredictProjectilePathParams TrajectoryParams;

	UPROPERTY(EditDefaultsOnly, Category=Trajectory, meta=(AllowPrivateAccess=true))
	UMaterialInstance* TrajectoryMaterialInstance;
	
private:
	UFUNCTION()
	void Throw();

	virtual void AttackStart() override;

	void SetAimMovement(bool IsAim) const;

	float Speed;

	FVector GetThrowingVelocity() const;	
	void InitSplineMeshes();
	FVector GetThrowingStartLocation();
	
protected:
	virtual void BeginPlay() override;
	virtual void AttackFinish() override;

	virtual void UnUse() override;
	void SetVisibleTrajectory(bool bVisible);
	virtual EWeaponType GetWeaponType() override { return EWeaponType::BOMB; };
	virtual ETriggerEvent GetCooldownOccurEvent() override { return ETriggerEvent::Completed; };

};
