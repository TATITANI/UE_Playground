// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CharacterWeaponComponent.h"
#include "MyGameInstance.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Inventory/WeaponInventory.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MyHUD.h"
#include "UI/IngameWidget.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Utils/UtilPlayground.h"

// Sets default values for this component's properties
UCharacterWeaponComponent::UCharacterWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCharacterWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	ProtagonistCharacter = Cast<AProtagonistCharacter>(GetOwner());
	if (ProtagonistCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner of UCharacterWeaponComponent was not found"));
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(ProtagonistCharacter->InputComponent))
		// Cast<UEnhancedInputComponent>(ProtagonistCharacter->GetLocalViewingPlayerController()->InputComponent))
	{
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &UCharacterWeaponComponent::ClickChangeWeapon);
	}

	WeaponInventory = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->WeaponInventory;
	ensure(WeaponInventory != nullptr);

	AWeaponActor* DefaultWeaponActor = Cast<AWeaponActor>(GetWorld()->SpawnActor(DefaultWeapon));
	ensure(DefaultWeaponActor != nullptr);
	ObtainWeapon(DefaultWeaponActor);
	
}



void UCharacterWeaponComponent::ClickChangeWeapon(const FInputActionValue& Value)
{
	if(CurrentWeapon->IsHidden())
		return;
	
	const int8 SlotID = static_cast<int8>(Value.Get<float>()) - 1;;
	const auto MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	const auto WeaponType = MyHUD->IngameWidget->GetSlotWeaponType(SlotID);
	// UE_LOG(LogTemp,Log,TEXT("ClickChangeWeapon : %d, %d"), SlotID, WeaponType);
	if (WeaponType != EWeaponType::WEAPON_None)
	{
		const auto WeaponActor = WeaponInventory->GetWeapon(WeaponType);
		ChangeWeapon(WeaponActor);
	}
}

void UCharacterWeaponComponent::ObtainWeapon(AWeaponActor* WeaponActor)
{
	ensure(WeaponActor);
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	WeaponActor->AttachToComponent(ProtagonistCharacter->GetMesh(), AttachmentRules, FName(WeaponActor->GetSocketName()));

	WeaponInventory->AddWeapon(WeaponActor);

	ChangeWeapon(WeaponActor);
}

void UCharacterWeaponComponent::ChangeWeapon(AWeaponActor* WeaponActor)
{
	if (WeaponInventory->HasWeapon(WeaponActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("%s weapon not in inventory"), *WeaponActor->GetName());
		return;
	}
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->UnEquip();
	}

	CurrentWeapon = WeaponActor;
	WeaponActor->Equip(ProtagonistCharacter);
	ProtagonistCharacter->CharacterCurrentInfo.SetCurrentWeaponType(WeaponActor->GetWeaponType());
	ProtagonistCharacter->AimCamByWeapon(WeaponActor->GetWeaponType());
	
	OnChangeWeapon.Broadcast(WeaponActor);
}

void UCharacterWeaponComponent::SetWeaponHidden(bool IsHidden) const
{
	CurrentWeapon->SetActorHiddenInGame(IsHidden);
}
