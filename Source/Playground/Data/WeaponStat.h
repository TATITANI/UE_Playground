// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "WeaponStat.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WEAPON_None UMETA(DisplayName="None"),
	SWORD UMETA(displayDisplayName = "Sword"),
	GUN UMETA(displayDisplayName = "Gun"),
	BOMB UMETA(displayDisplayName = "Bomb"),
};


USTRUCT(Atomic, BlueprintType)
struct FWeaponStat : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CoolTime = 0;

};


USTRUCT(Atomic, BlueprintType)
struct FGunStat : public FWeaponStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BulletCnt = 0;

};

USTRUCT(Atomic, BlueprintType)
struct FBombStat : public FWeaponStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

};

