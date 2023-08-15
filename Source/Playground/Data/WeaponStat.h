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


USTRUCT()
struct FWeaponTableRowBase : public FTableRowBase
{
	GENERATED_BODY()
};

USTRUCT(Atomic, BlueprintType)
struct FSwordStat : public FWeaponTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
};

USTRUCT(Atomic, BlueprintType)
struct FGunStat : public FWeaponTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BulletCnt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooltime;
};


USTRUCT(Atomic, BlueprintType)
struct FBombStat : public FWeaponTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooltime;
};
