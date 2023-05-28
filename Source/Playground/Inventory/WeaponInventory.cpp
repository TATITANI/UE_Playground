// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/WeaponInventory.h"

FWeaponInventory::FWeaponInventory()
{
}

FWeaponInventory::~FWeaponInventory()
{
}

bool FWeaponInventory::HasWeapon(AWeaponActor * WeaponActor) const
{
	return WeaponList.Contains(WeaponActor);
}

void FWeaponInventory::AddWeapon(AWeaponActor* WeaponActor)
{
	WeaponList.Add(WeaponActor);

}
