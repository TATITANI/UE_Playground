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
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Utils/UtilPlayground.h"

// Sets default values for this component's properties
UCharacterWeaponComponent::UCharacterWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.	
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UCharacterWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	PG_SUBLOG(LogPGNetwork, Log, TEXT(""));

	ProtagonistCharacter = Cast<AProtagonistCharacter>(GetOwner());

	checkf(ProtagonistCharacter != nullptr, TEXT("Owner of UCharacterWeaponComponent was not found"));

	WeaponInventory = NewObject<UWeaponInventory>(this);
	if (ProtagonistCharacter->HasAuthority())
	{
		DefaultWeaponActor = Cast<AWeaponActor>(GetWorld()->SpawnActor(DefaultWeaponClass));
		DefaultWeaponActor->SetOwner(ProtagonistCharacter->GetController());
	}
	else
	{
		if (ProtagonistCharacter->IsLocallyControlled())
		{
			if (UEnhancedInputComponent* EnhancedInputComponent =
				Cast<UEnhancedInputComponent>(ProtagonistCharacter->InputComponent))
			{
				EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &UCharacterWeaponComponent::ClickChangeWeapon);
			}

			WeaponInventory->AddOnObtainWeaponDelegate(
				FOnObtainWeapon::FDelegate::CreateUObject(this, &UCharacterWeaponComponent::ClientObtainWeaponEvent));

			WeaponInventory->OnClientAddWeapon(DefaultWeaponActor);

		}
	}

	// sub object replicate
	//AddReplicatedSubObject(WeaponInventory);

}

void UCharacterWeaponComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	if (WeaponInventory)
	{
		//RemoveRefplicatedSubObject(WeaponInventory);
	}

}

void UCharacterWeaponComponent::OnRep_WeaponInventory()
{
	PG_SUBLOG(LogPGNetwork, Warning, TEXT(""));
	ensureAlways(WeaponInventory->IsValidLowLevel());
	//WeaponInventory->OnObtainWeapon.AddUObject(this, &UCharacterWeaponComponent::ClientObtainWeaponEvent);

}


void UCharacterWeaponComponent::OnRep_DefaultWeaponActor()
{
	PG_SUBLOG(LogPGNetwork, Warning, TEXT(""));
	// ServerObtainWeapon(DefaultWeaponActor);
}



void UCharacterWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UCharacterWeaponComponent, DefaultWeaponActor, COND_InitialOnly);
	DOREPLIFETIME(UCharacterWeaponComponent, WeaponInventory);
}

void UCharacterWeaponComponent::ClickChangeWeapon(const FInputActionValue& Value)
{
	if (CurrentWeapon->IsHidden())
		return;

	const int8 SlotID = static_cast<int8>(Value.Get<float>()) - 1;;
	const auto MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	const auto WeaponType = MyHUD->IngameWidget->GetSlotWeaponType(SlotID);
	if (WeaponType != EWeaponType::WEAPON_None)
	{
		const auto WeaponActor = WeaponInventory->GetWeapon(WeaponType);
		ChangeWeapon(WeaponActor);
	}
}

void UCharacterWeaponComponent::AttachWeapon(AWeaponActor* WeaponActor) const
{
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	WeaponActor->AttachToComponent(ProtagonistCharacter->GetBodyMesh(), AttachmentRules, FName(WeaponActor->GetSocketName()));
}


void UCharacterWeaponComponent::ServerObtainWeapon_Implementation(AWeaponActor* WeaponActor)
{
	if (ensureAlways(WeaponActor) == false)
		return;

	if (ensureAlways(ProtagonistCharacter) == false)
		return;

	PG_SUBLOG(LogPGNetwork, Log, TEXT(""));
	// AttachWeapon(WeaponActor);
	// WeaponInventory->OnClientAddWeapon(WeaponActor);
}


void UCharacterWeaponComponent::ClientObtainWeaponEvent(AWeaponActor* WeaponActor)
{
	PG_SUBLOG(LogPGNetwork, Warning, TEXT(""));
	if (WeaponInventory == nullptr)
	{
		PG_SUBLOG(LogPGNetwork, Error, TEXT("WeaponInven null"));
		return;
	}

	ChangeWeapon(WeaponActor);
	if (ObtainSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ObtainSound, WeaponActor->GetActorLocation());
	}

	ServerObtainWeapon(WeaponActor);
}


void UCharacterWeaponComponent::ChangeWeapon(AWeaponActor* WeaponActor)
{
	if (WeaponInventory->HasWeapon(WeaponActor) == false)
	{
		PG_SUBLOG(LogPGNetwork, Error, TEXT("inventory has not [ %s ] weapon"), *WeaponActor->GetName());
		return;
	}

	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->UnEquip();
	}

	CurrentWeapon = WeaponActor;
	CurrentWeapon->Equip(ProtagonistCharacter);

	ProtagonistCharacter->CharacterCurrentInfo.SetCurrentWeaponType(CurrentWeapon->GetWeaponType());
	ProtagonistCharacter->AimCamByWeapon(CurrentWeapon->GetWeaponType());

	OnChangeWeapon.Broadcast(CurrentWeapon);

}

void UCharacterWeaponComponent::SetWeaponHidden(bool IsHidden) const
{
	CurrentWeapon->SetActorHiddenInGame(IsHidden);
}
