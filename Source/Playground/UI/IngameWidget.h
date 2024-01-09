// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameWidget.generated.h"

enum class EWeaponType : uint8;
/**
 * 
 */
UCLASS()
class PLAYGROUND_API UIngameWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true)) // c++ 에서 widget 블루프린트 접근
	class UImage* Img_HP_Gauge; // 에디터 상 컴포넌트 이름과 같아야 함.

	UMaterialInstanceDynamic* MaterialInstanceDynamic;

	
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true))
	class UHorizontalBox* HB_Weapon;

	UPROPERTY()
	class AProtagonistCharacter* ProtagonistCharacter;

	UFUNCTION()
	void UpdateHp(int32 Hp, int32 MaxHp) const;

	UPROPERTY(EditAnywhere, Category=Weapon, meta=(AllowPrivateAccess=true))
	TMap<EWeaponType, UTexture2D*> WeaponImageTable;

	uint8 NewWeaponIndex = 0;

	class UWeaponSlotWidget* CurrentWeaponSlot;

protected:
	virtual void NativeConstruct() override;
public:
	UFUNCTION()
	void ChangeCurrentWeapon(class AWeaponActor* WeaponActor) ;
	
	UFUNCTION()
	void AddWeapon(class AWeaponActor* WeaponActor);

	UFUNCTION()
	void Cooldown(double BeginSeconds, double EndSeconds);

	UFUNCTION()
	void UseWeapon(int32 RemainCnt, int32 MaxCnt);
	
	EWeaponType GetSlotWeaponType(int8 SlotID);
};
