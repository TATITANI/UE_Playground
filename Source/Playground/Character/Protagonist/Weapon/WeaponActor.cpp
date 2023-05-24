// Copyright Epic Games, Inc. All Rights Reserved.


#include "WeaponActor.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Component/StatComponent.h"


// Sets default values for this component's properties
AWeaponActor::AWeaponActor()
{
}

void AWeaponActor::AttachWeapon(AProtagonistCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	ensure(Character != nullptr);
	Character->CharacterCurrentInfo->SetCurrentWeaponType(GetWeaponType());
	AnimInstance = Cast<UProtagonistAnimInstance>(Character->GetMesh()->GetAnimInstance());
	ensure(AnimInstance!=nullptr);

	SetupInput();
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh(), AttachmentRules, FName(SocketName));
	const auto StatComp = Cast<UStatComponent>(Character->FindComponentByClass(UStatComponent::StaticClass()));
	ensure(StatComp);
	this->Damage = StatComp->GetDamage();

	AttachEvent();
}

void AWeaponActor::SetupInput()
{
	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer());
		ensure(Subsystem != nullptr);
		Subsystem->AddMappingContext(InputMappingContext, InputPriority);

		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
		ensure(EnhancedInputComponent != nullptr);
		BindInputActions(EnhancedInputComponent);
	}
}

void AWeaponActor::AttachEvent()
{
}


void AWeaponActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}
