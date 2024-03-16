// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BotGenerator.h"

#include "BotAIController.h"
#include "Character/Bot/BotCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ABotGenerator::ABotGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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

void ABotGenerator::SetActiveBot(ABotCharacter* Bot, bool IsActive)
{
	Bot->SetActorHiddenInGame(!IsActive);
	Bot->SetActorEnableCollision(IsActive);
	Bot->SetActorTickEnabled(IsActive);

	Bot->GetCharacterMovement()->GravityScale = IsActive ? 1 : 0 ;
	if(!IsActive)
	{
		ABotAIController* BotAIController = Bot->GetController<ABotAIController>();
		BotAIController->StopBehaviorTree("Inactive");
	}
	
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
