// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "CharacterWeaponComponent.generated.h"



DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeWeapon, class AWeaponActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUseWeapon, int32 /* remainCnt */, int32 /* MaxCnt */);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLAYGROUND_API UCharacterWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category=Weapon, meta=(AllowPrivateAccess=true))
	TSubclassOf<class AWeaponActor> DefaultWeapon;

	class AWeaponActor* CurrentWeapon;

	class AProtagonistCharacter *ProtagonistCharacter;
	
	TSharedPtr<class FWeaponInventory> WeaponInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponAction;
	
	void ClickChangeWeapon(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly, Category= Sound, meta=(AllowPrivateAccess))
	USoundBase* ObtainSound;

public:
	FOnChangeWeapon OnChangeWeapon;
	FOnCooldown OnCooldownWeapon;
	FOnUseWeapon OnUseWeapon;
	
	void ObtainWeapon(class AWeaponActor* WeaponActor);
	void ChangeWeapon(class AWeaponActor* WeaponActor);
	void SetWeaponHidden(bool IsHidden) const;
	
};
