// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GunActor.h"

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
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireMontage, 1.f);
		}
	}
}

void AGunActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(InputMappingContext);
		}
	}
}

void AGunActor::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(FireInputAction, ETriggerEvent::Triggered, this, &AGunActor::Fire);

}

void AGunActor::AttachEvent()
{
	Super::AttachEvent();
	
	Character->SetHasRifle(true);
}