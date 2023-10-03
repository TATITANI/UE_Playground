// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Save/PlaygroundSaveGame.h"


UMyGameInstance::UMyGameInstance()
{

}

void UMyGameInstance::Init()
{
	Super::Init();
	WeaponInventory = MakeShared<FWeaponInventory>();
	ItemInventory = NewObject<UItemInventory>();

	LoadSavingData();
}

void UMyGameInstance::LoadSavingData()
{
	UPlaygroundSaveGame* SavingData = Cast<UPlaygroundSaveGame>(UGameplayStatics::LoadGameFromSlot(SavingSlotName, 0));
	if (nullptr == SavingData)
	{
		SavingData = GetMutableDefault<UPlaygroundSaveGame>(); // CDO 클래스 반환
	}

	ItemInventory->Init(SavingData->ItemTable);

}

void UMyGameInstance::Save()
{
	UPlaygroundSaveGame* SavingData = NewObject<UPlaygroundSaveGame>();
	SavingData->ItemTable = ItemInventory->GetTable();

	if (UGameplayStatics::SaveGameToSlot(SavingData, SavingSlotName, 0))
	{
		UE_LOG(LogClass, Log, TEXT("SaveGame Success"));
	}
	else
	{
		UE_LOG(LogClass, Error, TEXT("SaveGame Error!"));
	}
}

