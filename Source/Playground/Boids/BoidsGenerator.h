// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidsGenerator.generated.h"

UCLASS()
class PLAYGROUND_API ABoidsGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidsGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	class TSubclassOf<class ABoidEntity> BoidEntity;

	TArray<ABoidEntity*> ListEntity;

	UPROPERTY(EditDefaultsOnly, Category=Boids, meta=(AllowPrivateAccess=true))
	float MovableRadius = 10000;

	virtual bool ShouldTickIfViewportsOnly() const override;
};




