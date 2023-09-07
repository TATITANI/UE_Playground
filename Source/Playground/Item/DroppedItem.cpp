// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DroppedItem.h"

#include "MyGameInstance.h"
#include "PlaygroundItem.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADroppedItem::ADroppedItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
}

// Called when the game starts or when spawned
void ADroppedItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADroppedItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADroppedItem::Init(FVector Loc)
{
	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Item = GameInstance->GetDroppedItem();
	ItemCnt = FMath::RandRange(1, Item->MaxCount);

	auto SM = Item->ObjectStaticMesh.LoadSynchronous();
	StaticMeshComponent->SetStaticMesh(SM);

	SetActorLocation(Loc);
}

