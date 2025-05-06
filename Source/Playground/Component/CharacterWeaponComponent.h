// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Inventory/WeaponInventory.h"
#include "CharacterWeaponComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeWeapon, class AWeaponActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUseWeapon, int32 /* remainCnt */, int32 /* MaxCnt */);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UCharacterWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	TSubclassOf<class AWeaponActor> DefaultWeaponClass;

	UPROPERTY(ReplicatedUsing = OnRep_DefaultWeaponActor)
	AWeaponActor* DefaultWeaponActor;

	UFUNCTION()
	void OnRep_DefaultWeaponActor();



	UPROPERTY()
	TObjectPtr<AWeaponActor> CurrentWeapon;

	UPROPERTY()
	class AProtagonistCharacter* ProtagonistCharacter;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponInventory)
	UWeaponInventory* WeaponInventory;

	UFUNCTION()
	void OnRep_WeaponInventory();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponAction;


	UPROPERTY(EditDefaultsOnly, Category = Sound, meta = (AllowPrivateAccess))
	USoundBase* ObtainSound;


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ClickChangeWeapon(const FInputActionValue& Value);

	UFUNCTION()
	void OnClientObtainWeapon(AWeaponActor* WeaponActor);

	void ChangeWeapon(class AWeaponActor* WeaponActor);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	FOnChangeWeapon OnChangeWeapon;
	FOnCooldown OnCooldownWeapon;
	FOnUseWeapon OnUseWeapon;

	// called on server
	UFUNCTION(Server, Reliable)
	void ServerObtainWeapon(class AWeaponActor* WeaponActor);


	void SetWeaponHidden(bool IsHidden) const;


};
