// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "FlyingBomb.generated.h"

UCLASS()
class PLAYGROUND_API AFlyingBomb : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyingBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	class USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	class UParticleSystem* ExplodeParticleSystem;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	USoundBase* ExplodeSound;

	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	int32 Damage;

public:
	void Fly(FVector Velocity, int32 _Damage);
	float GetRadius() { return SphereComponent->GetScaledSphereRadius(); };
};
