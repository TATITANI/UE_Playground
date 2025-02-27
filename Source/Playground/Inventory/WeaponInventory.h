// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
/**
 * 
 */
#include "WeaponInventory.generated.h"


enum class EWeaponType : uint8;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnObtainWeapon, class AWeaponActor*);

USTRUCT()
struct FWeaponEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:

	// todo 액터대신 무기데이터로 
	UPROPERTY()
	AWeaponActor* WeaponActor;

	FWeaponEntry() : WeaponActor(nullptr)
	{
	}

	void PostReplicatedAdd(const struct FFastArraySerializer& InArraySerializer) ;
};

USTRUCT()
struct FWeaponList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FWeaponEntry> Items;

	TWeakObjectPtr<UWeaponInventory> OwnerWeaponInventory = nullptr;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FWeaponEntry, FWeaponList>(Items, DeltaParams, *this);
	}

	// called in client
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);

};

template <>
struct TStructOpsTypeTraits<FWeaponList> : public TStructOpsTypeTraitsBase2<FWeaponList>
{
	enum { WithNetDeltaSerializer = true };
};

UCLASS(Blueprintable)
class PLAYGROUND_API UWeaponInventory : public UObject 
{
	GENERATED_BODY()
	
	friend struct FWeaponList;
	
public:
	UWeaponInventory();
	
private:
	
	UPROPERTY(ReplicatedUsing= OnRep_WeaponList, VisibleAnywhere, Category=Weapon, meta=(AllowPrivateAccess = true))
	FWeaponList WeaponList;

	UFUNCTION()
	void OnRep_WeaponList();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsSupportedForNetworking() const override;


public:
	FOnObtainWeapon OnObtainWeapon;

	bool HasWeapon(class AWeaponActor* WeaponActor) const;

	bool HasWeapon(EWeaponType WeaponType) const;

	UFUNCTION()
	void AddWeapon(class AWeaponActor* WeaponActor);

	AWeaponActor* GetWeapon(EWeaponType WeaponType);

	FWeaponList GetWeaponList() { return WeaponList; }

	void AddOnObtainWeaponDelegate(const FOnObtainWeapon::FDelegate& InHandler);

	void RemoveOnObtainWeaponDelegate(FDelegateHandle DelegateHandle);
	
};
