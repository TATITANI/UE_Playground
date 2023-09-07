// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterStat.h"
#include "Engine/GameInstance.h"
#include "Inventory/WeaponInventory.h"
#include "Item/PlaygroundItem.h"
#include "MyGameInstance.generated.h"

enum class ECharacterStatType : uint8;

UCLASS()
class PLAYGROUND_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category=Stat, meta=(AllowPrivateAccess=true))
	TMap<ECharacterStatType, UDataTable*> CharacterStatMap;

	UPROPERTY(EditAnywhere, Category=Stat, meta=(AllowPrivateAccess=true))
	TMap<EWeaponType, UDataTable*> WeaponStatMap;
	
	UPROPERTY(EditAnywhere, Category=Item, meta=(AllowPrivateAccess=true))
	TArray<UPlaygroundItem *> ItemList;
	

public:
	UMyGameInstance();
	virtual void Init() override;
	template <class T>
	TOptional<T> GetCharacterStat(ECharacterStatType StatType, FName RowName);

	template <class T>
	TOptional<T> GetWeaponStat(EWeaponType StatType, FName RowName);

	TSharedPtr<FWeaponInventory> WeaponInventory;

	UPlaygroundItem* GetDroppedItem();

	struct FStreamableManager* StreamableManager;
};

template <class T>
TOptional<T> UMyGameInstance::GetCharacterStat(ECharacterStatType StatType, FName RowName)
{
	UDataTable* DataTable = *CharacterStatMap.Find(StatType);
	ensureMsgf(DataTable != nullptr, TEXT("Data Table not exist : %s"), *UEnum::GetValueAsString(StatType));

	const T* Data = DataTable->FindRow<T>(RowName,TEXT(""));
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable type not match : %s"), *UEnum::GetValueAsString(StatType));
		return NullOpt;
	}

	return *Data;
}

template <class T>
TOptional<T> UMyGameInstance::GetWeaponStat(EWeaponType StatType, FName RowName)
{
	UDataTable* DataTable = *WeaponStatMap.Find(StatType);
	ensureMsgf(DataTable != nullptr, TEXT("Data Table not exist : %s"), *UEnum::GetValueAsString(StatType));

	const T* Data = DataTable->FindRow<T>(RowName,TEXT(""));
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable type not match : %s"), *UEnum::GetValueAsString(StatType));
		return NullOpt;
	}

	return *Data;
}
