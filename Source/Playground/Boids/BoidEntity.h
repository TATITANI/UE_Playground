// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoidsGenerator.h"
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

	void Init(ABoidEntity* _Leader, FVector _Pivot, float _MovableRadius, float _Speed, const FBoidsWeight _BoidsWeight);

private:

	ABoidEntity* Leader;
	FVector Pivot;
	FBoidsWeight BoidsWeight;
	float MovableRadius;

	float Speed;

	UPROPERTY(EditDefaultsOnly, Category=Trace, meta=(AllowPrivateAccess=true))
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel = ECC_WorldDynamic;

	FVector TargetVelocity;
	UPROPERTY(VisibleAnywhere, Category=Moving, meta=(AllowPrivateAccess= true))
	FVector CurrentVelocity = FVector::ZeroVector;
	FVector RandomVector;


	void UpdateRandomMovement();
	bool CheckObstacle(FHitResult& HitResult);

public:
	void CalculateDir();
	void SetTargetVelocity(FVector _Velocity) { TargetVelocity = _Velocity; }
	virtual FVector GetVelocity() const override { return CurrentVelocity; }

	void SetRandomVec(FVector _RandomVec) { RandomVector = _RandomVec; }
};
