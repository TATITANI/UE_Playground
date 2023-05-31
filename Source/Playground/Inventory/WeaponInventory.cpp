// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/WeaponInventory.h"

#include "Character/Protagonist/Weapon/WeaponActor.h"

FWeaponInventory::FWeaponInventory()
{
}

FWeaponInventory::~FWeaponInventory()
{
}

bool FWeaponInventory::HasWeapon(AWeaponActor* WeaponActor) const
{
	return WeaponList.Contains(WeaponActor);
}

bool FWeaponInventory::HasWeapon(EWeaponType WeaponType) const
{
	return WeaponList.ContainsByPredicate([WeaponType](AWeaponActor* Weapon)
	{
		return Weapon->GetWeaponType() == WeaponType;
	});
}

void FWeaponInventory::AddWeapon(AWeaponActor* WeaponActor)
{
	WeaponList.Add(WeaponActor);
}

AWeaponActor* FWeaponInventory::GetWeapon(EWeaponType WeaponType)
{
	AWeaponActor** WeaponActor = WeaponList.FindByPredicate([&](AWeaponActor *Weapon)
	{
		return Weapon->GetWeaponType() == WeaponType;
	});
	return *WeaponActor;
}
