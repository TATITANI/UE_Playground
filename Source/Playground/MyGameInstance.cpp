// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
	Super::Init();
	WeaponInventory = MakeShared<FWeaponInventory>();
	// ItemInventory = MakeShared<UItemInventory>();
	ItemInventory = NewObject<UItemInventory>();

}

