// Copyright Epic Games, Inc. All Rights Reserved.


#include "WeaponComponent.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Character/Protagonist/ProtagonistProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "StatComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
}

void UWeaponComponent::Fire()
{
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();

		if (World != nullptr)
		{
			const auto Arrow = Cast<USceneComponent>(GetOwner()->FindComponentByClass(UArrowComponent::StaticClass()));
			const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			if (Arrow != nullptr || PlayerController != nullptr)
			{
				//world trf
				const FVector ProjectilePos = Arrow->GetComponentLocation();
				// auto ProjectileRot = Arrow->GetComponentRotation();
				const auto ProjectileRot = PlayerController->PlayerCameraManager->GetCameraRotation();

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// Spawn the projectile at the muzzle
				const auto Projectile = World->SpawnActor<AProtagonistProjectile>(ProjectileClass, ProjectilePos, ProjectileRot, ActorSpawnParams);
				Projectile->Init(this->Damage);
			}
		}
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UWeaponComponent::AttachWeapon(AProtagonistCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	ensure(Character != nullptr);

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh(), AttachmentRules, FName(GunSocketName));

	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);
	const auto StatComp = Cast<UStatComponent>(Character->FindComponentByClass(UStatComponent::StaticClass()));
	ensure(StatComp);
	this->Damage = StatComp->GetDamage();

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UWeaponComponent::Fire);
		}
	}
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}
