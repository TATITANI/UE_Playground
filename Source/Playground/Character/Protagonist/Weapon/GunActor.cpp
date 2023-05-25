// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protagonist/Weapon/GunActor.h"

#include "EnhancedInputSubsystems.h"
#include "Character/Protagonist/ProtagonistProjectile.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"


void AGunActor::Fire()
{
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const auto Arrow = Cast<USceneComponent>(FindComponentByClass(UArrowComponent::StaticClass()));
			ensure(Arrow!= nullptr);
			const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			ensure(PlayerController != nullptr);

			if (Arrow != nullptr || PlayerController != nullptr)
			{

				//world trf
				const FVector ProjectilePos = Arrow->GetComponentLocation();
				const auto ProjectileRot = Character->GetActorRotation();

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
	if (FireMontage != nullptr)
	{
		AnimInstance->Montage_Play(FireMontage, 1.f);
	}
}


void AGunActor::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(FireInputAction, ETriggerEvent::Triggered, this, &AGunActor::Fire);
}
