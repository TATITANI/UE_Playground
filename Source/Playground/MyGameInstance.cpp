// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
	Super::Init();
	WeaponInventory = MakeShared<FWeaponInventory>();

}

UPlaygroundItem* UMyGameInstance::GetDroppedItem()
{
	float AccumWeight = 0;
	for(auto Item : ItemList)
	{
		AccumWeight += Item->DroppedWeight;
	}

	const float TargetWeight = FMath::FRandRange(0, AccumWeight); 
	float Weight = 0;
	for(auto Item : ItemList)
	{
		Weight += Item->DroppedWeight;
		if(Weight >= TargetWeight)
		{
			return Item;
		}
	}

	return ItemList.Last();
	
}

