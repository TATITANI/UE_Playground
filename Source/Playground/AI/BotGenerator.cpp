// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BotGenerator.h"

#include "Character/Bot/BotCharacter.h"

// Sets default values
ABotGenerator::ABotGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABotGenerator::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < SpawnCapacity; i++)
	{
		ABotCharacter* Bot = GetWorld()->SpawnActor<ABotCharacter>(SubclassOfBot);
		Bot->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		SetActiveBot(Bot, false);
		Pool.Push(Bot);
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandler, this, &ABotGenerator::TakeBot, SpawnCycle, true, 0);
}

void ABotGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABotGenerator::SetActiveBot(ABotCharacter* Bot, bool IsActive)
{
	Bot->SetActorHiddenInGame(!IsActive);
	Bot->SetActorEnableCollision(IsActive);
	Bot->SetActorTickEnabled(IsActive);
}

void ABotGenerator::TakeBot()
{
	if (Pool.IsEmpty())
		return;

	auto Bot = Pool.Pop();
	SetActiveBot(Bot, true);

	const int LocID = FMath::RandRange(0, ArrSpawnLoc.Num() - 1);
	const auto Loc = GetActorLocation() + ArrSpawnLoc[LocID];
	Bot->Init(this, Loc);
}

void ABotGenerator::ReturnBot(ABotCharacter* Bot)
{
	Pool.Push(Bot);
	SetActiveBot(Bot, false);
}
