// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProtagonistProjectile.generated.h"

class USphereComponent;

UCLASS(config=Game)
class AProtagonistProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	class UNiagaraSystem* ExplodeParticleSystem;

public:
	
	/** Projectile movement component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	class UProjectileMovementComponent* ProjectileMovement;
	
private:
	float Damage = 0;

public:
	AProtagonistProjectile();
	virtual void BeginPlay() override;

public:
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void Init(float& _Damage) { this->Damage = _Damage; }
};
