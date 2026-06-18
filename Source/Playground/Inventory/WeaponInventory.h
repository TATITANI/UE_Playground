// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/WeaponInfo.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "WeaponInventory.generated.h"


enum class EWeaponType : uint8;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnObtainWeapon, EWeaponType);

USTRUCT()
struct FWeaponInfoEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UWeaponInfo> WeaponInfo;


	// 복제 후 클라이언트에서 추가될 때 호출
	void PostReplicatedAdd(const struct FFastArraySerializer& InArraySerializer);
};

USTRUCT()
struct FWeaponInfoList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FWeaponInfoEntry> WeaponInfos;

	//TWeakObjectPtr<UWeaponInventory> OwnerWeaponInventory = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize
		<FWeaponInfoEntry, FWeaponInfoList>(WeaponInfos, DeltaParams, *this);
	}

	// called in client
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
};

template <>
struct TStructOpsTypeTraits<FWeaponInfoList> : public TStructOpsTypeTraitsBase2<FWeaponInfoList>
{
	enum { WithNetDeltaSerializer = true };
};

/***************************************/


UCLASS(Blueprintable)
class PLAYGROUND_API UWeaponInventory : public UObject
{
	GENERATED_BODY()

	//friend struct FWeaponInfoList;

public:
	UWeaponInventory();

private:


	//----------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_WeaponInfoList)
	FWeaponInfoList HasWeaponInfoList;
	//----------------------------------------

	// client only
	//----------------------------------------
	UPROPERTY()
	TArray<AWeaponActor*> WeaponList;
	//----------------------------------------

	virtual void PostNetReceive() override;

	 UFUNCTION()
	 void OnRep_WeaponInfoList();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsSupportedForNetworking() const override;

	FOnObtainWeapon OnObtainWeapon;

public:
	bool HasWeapon(class AWeaponActor* WeaponActor) const;

	bool HasWeapon(EWeaponType WeaponType) ;

	UFUNCTION()
	void OnClientAddWeapon(class AWeaponActor* WeaponActor);

	UFUNCTION()
	void OnServerAddWeapon(UWeaponInfo* WeaponInfo);


	AWeaponActor* GetWeapon(EWeaponType WeaponType);

	// FWeaponList GetWeaponList() { return WeaponList; }
	TArray<AWeaponActor*> GetWeaponList() { return WeaponList; }

	void AddOnObtainWeaponDelegate(const FOnObtainWeapon::FDelegate& InHandler);

	void RemoveOnObtainWeaponDelegate(FDelegateHandle DelegateHandle);
};




