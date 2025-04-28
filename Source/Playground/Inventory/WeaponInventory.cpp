// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/WeaponInventory.h"

#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Engine/PackageMapClient.h"
#include "Net/UnrealNetwork.h"

void FWeaponInfoEntry::PostReplicatedAdd(const FFastArraySerializer& InArraySerializer)
{
	// UE_LOG(LogPGNetwork, Log, TEXT("FWeaponEntry::PostReplicatedAdd("));
}


void FWeaponInfoList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	if (OwnerWeaponInventory == nullptr)
	{
		UE_LOG(LogPGNetwork, Error, TEXT("FWeaponList::PostReplicatedAdd owner null"));
		return;
	}

#if 1 // netmode check - client에서 출력됨	
	FString StrNetmode;
	switch (OwnerWeaponInventory->GetWorld()->GetNetMode())
	{
	case NM_Standalone:
		StrNetmode = TEXT("Standalone");
		break;
	case NM_ListenServer:
		StrNetmode = TEXT("ListenServer");
		break;
	case NM_DedicatedServer:
		StrNetmode = TEXT("DedicatedServer");
		break;
	case NM_Client:
		StrNetmode = TEXT("Client");
		break;
	default:
		StrNetmode = TEXT("Unknown");
		break;
	}
	UE_LOG(LogPGNetwork, Warning, TEXT("[%s] FWeaponList::PostReplicatedAdd"), *StrNetmode);;
#endif


	for (auto id : AddedIndices)
	{
	/*	UE_LOG(LogPGNetwork, Log, TEXT("FWeaponList::PostReplicatedAdd - id : %d/ actorName : %s"),
		       id, *Items[id].WeaponActor->GetName());*/


		// 1) 인벤토리에 무기 액터가 있긴있어야됨. 빠른 교체를 위해서.
		// 2) replicate는 액터가 아닌 무기정보로 해야됨

		if (OwnerWeaponInventory->OnObtainWeapon.IsBound())
		{
			UE_LOG(LogPGNetwork, Log, TEXT("PostReplicatedAdd - WeaponActor : %s"), *Items[id].WeaponActor->GetName());
			OwnerWeaponInventory->OnObtainWeapon.Broadcast(Items[id].WeaponActor);
		}
		else
		{
			UE_LOG(LogPGNetwork, Error, TEXT("OwnerWeaponInventory->OnObtainWeapon not bound"));
		}
	}
}

UWeaponInventory::UWeaponInventory()
{
	// WeaponList.OwnerWeaponInventory = this;
}

bool UWeaponInventory::IsSupportedForNetworking() const
{
	return true;
}

 void UWeaponInventory::OnRep_WeaponInfoList()
 {
 	UE_LOG(LogPGNetwork, Warning, TEXT("UWeaponInventory::OnRep_WeaponList"));

 	auto Entries = HasWeaponInfos.WeaponInfos;
 	for (auto Entry : Entries)
 	{
 		UE_LOG(LogPGNetwork, Warning, TEXT("UWeaponInventory::OnRep_WeaponList - %s"), *Entry.WeaponActor->GetName());
 	}
 }

void UWeaponInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponInventory, WeaponList);
}


bool UWeaponInventory::HasWeapon(AWeaponActor* WeaponActor) const
{
	UE_LOG(LogPGNetwork, Warning, TEXT("UWeaponInventory::HasWeapon"));

	return WeaponList.Contains(WeaponActor);

	// auto Entries = WeaponList.Items;
	// for (auto Entry : Entries)
	// {
	// 	UE_LOG(LogPGNetwork, Warning, TEXT("UWeaponInventory::HasWeapon - %s"), *Entry.WeaponActor->GetName());
	// }
	//
	//
	// return WeaponList.Items.FindByPredicate([WeaponActor](const FWeaponEntry& Entry)
	// {
	// 	return Entry.WeaponActor == WeaponActor;
	// }) != nullptr;
}

bool UWeaponInventory::HasWeapon(EWeaponType WeaponType) 
{

	AWeaponActor* Weapon = *WeaponList.FindByPredicate([WeaponType](AWeaponActor* WeaponActor)
	{
		return WeaponActor->GetWeaponType() == WeaponType;
	});

	return Weapon != nullptr;

	// return WeaponList.Items.FindByPredicate([WeaponType](const FWeaponEntry& Entry)
	// {
	// 	return Entry.WeaponActor && Entry.WeaponActor->GetWeaponType() == WeaponType;
	// }) != nullptr;
}

void UWeaponInventory::OnClientAddWeapon(AWeaponActor* WeaponActor)
{
	if (!WeaponActor || HasWeapon(WeaponActor))
	{
		return;
	}

	WeaponList.Add(WeaponActor);
	OnObtainWeapon.Broadcast(WeaponActor);
	
	// FWeaponEntry NewEntry;
	// NewEntry.WeaponActor = WeaponActor;
	// WeaponList.Items.Add(NewEntry);
	// WeaponList.MarkItemDirty(NewEntry);
}

void UWeaponInventory::OnServerAddWeapon(AWeaponActor* WeaponActor)
{
	if(ensureAlwaysMsgf(WeaponActor != nullptr, TEXT("WeaponActor Nullptr")) == false)
	{
		return;
	}

	if(ensureAlwaysMsgf(HasWeapon(WeaponActor), TEXT("WeaponActor Nullptr")) == false)
	{
		return;
	}

	WeaponList.Add(WeaponActor);
}

AWeaponActor* UWeaponInventory::GetWeapon(EWeaponType WeaponType)
{
	AWeaponActor* FoundWeapon = *WeaponList.FindByPredicate([&](AWeaponActor* WeaponActor)
	{
		return WeaponActor->GetWeaponType() == WeaponType;
	});

	// const FWeaponEntry* FoundEntry = WeaponList.Items.FindByPredicate([&](const FWeaponEntry& Entry)
	// {
	// 	return Entry.WeaponActor && Entry.WeaponActor->GetWeaponType() == WeaponType;
	// });

	return FoundWeapon ? FoundWeapon : nullptr;
}

void UWeaponInventory::AddOnObtainWeaponDelegate(const FOnObtainWeapon::FDelegate& InHandler)
{
	OnObtainWeapon.Add(InHandler);
}

void UWeaponInventory::RemoveOnObtainWeaponDelegate(FDelegateHandle DelegateHandle)
{
	OnObtainWeapon.Remove(DelegateHandle);
}
