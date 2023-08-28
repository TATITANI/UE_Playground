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
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CoolTime;

};


USTRUCT(Atomic, BlueprintType)
struct FGunStat : public FWeaponStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BulletCnt;

};

USTRUCT(Atomic, BlueprintType)
struct FBombStat : public FWeaponStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

};

