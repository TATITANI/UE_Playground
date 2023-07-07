// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidEntity.generated.h"

UCLASS()
class PLAYGROUND_API ABoidEntity : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABoidEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(ABoidEntity* _Leader, FVector _Pivot, float _MovableRadius);
	
private:
	ABoidEntity* Leader;
	FVector Pivot;
	float MovableRadius;


	UPROPERTY(EditDefaultsOnly, Category=Move, meta=(AllowPrivateAccess=true))
	float Speed = 100;


	UPROPERTY(EditDefaultsOnly, Category=Trace, meta=(AllowPrivateAccess=true))
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel = ECC_WorldDynamic;


	UPROPERTY(EditDefaultsOnly, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightAlignment = 1;

	UPROPERTY(EditDefaultsOnly, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightSeperation = 1;

	UPROPERTY(EditDefaultsOnly, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightCohesion = 1;

	UPROPERTY(EditDefaultsOnly, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightRandomMove = 1;

	UPROPERTY(EditDefaultsOnly, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightLeaderFollowing = 1;

	FVector Velocity = FVector::ZeroVector;

	FVector RandomVector;
	FTimerHandle RandomMovementTimerHandle;
	void UpdateRandomMovement();

	FVector CalculateDir();

	bool CheckObstacle(FHitResult &HitResult);
public:
	virtual FVector GetVelocity() const override { return Velocity; }
};
