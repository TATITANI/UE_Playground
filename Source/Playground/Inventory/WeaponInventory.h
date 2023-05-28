// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class PLAYGROUND_API FWeaponInventory
{
private:
	UPROPERTY(VisibleAnywhere, Category=Weapon, meta=(AllowPrivateAccess = true))
	TArray< class AWeaponActor*> WeaponList;
	
public:
	FWeaponInventory();
	~FWeaponInventory();

	bool HasWeapon(class AWeaponActor * WeaponActor) const;
	void AddWeapon(class AWeaponActor * WeaponActor);
};
