// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PlaygroundItem.h"

#include "DroppedItemTable.h"
#include "MyGameInstance.h"
#include "PlaygroundGameMode.h"
#include "ItemData.h"
#include "ItemInventory.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlaygroundItem::APlaygroundItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSimulatePhysics(true);
	SetRootComponent(SphereComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(SphereComponent);

}
	
void APlaygroundItem::PostInitProperties()
{
	Super::PostInitProperties();
	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemData null"));
	}
}


// Called when the game starts or when spawned
void APlaygroundItem::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &APlaygroundItem::OnSphereBeginOverlap);
}


// Called every frame
void APlaygroundItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlaygroundItem::Init(FVector Loc, FItemStatus _ItemStatus)
{
	ItemStatus = _ItemStatus;
	SetActorLocation(Loc);
}


void APlaygroundItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AProtagonistCharacter::StaticClass()))
	{
		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		GameInstance->ItemInventory->AddItem(this->ItemData, ItemStatus.Count);
		
		UE_LOG(LogTemp, Log, TEXT("Obtain Item %d"), ItemStatus.Count);

		Destroy();
	}
}
