// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidsGenerator.h"

#include "BoidEntity.h"

// Sets default values
ABoidsGenerator::ABoidsGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled= true;

}

// Called when the game starts or when spawned
void ABoidsGenerator::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters ActorSpawnParams;

	for (int i = 0; i < 200; i++)
	{
		double dx = FMath::RandRange(-1500, 1500);
		double dy = FMath::RandRange(-1500, 1500);
		double dz = FMath::RandRange(-1500, 1500);
		auto Entity = GetWorld()->SpawnActor<ABoidEntity>(BoidEntity, GetActorLocation() + FVector(dx, dy, dz),
		                                                  FRotator::ZeroRotator, ActorSpawnParams);
		ListEntity.Add(Entity);
		Entity->Init(ListEntity[0], GetActorLocation(), MovableRadius);
		Entity->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), MovableRadius, 32, FColor::Turquoise, true);
}

// Called every frame
void ABoidsGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


bool ABoidsGenerator::ShouldTickIfViewportsOnly() const
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), MovableRadius, 32, FColor::Turquoise, false);
	return Super::ShouldTickIfViewportsOnly();
}

