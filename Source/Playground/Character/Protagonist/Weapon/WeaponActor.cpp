// Copyright Epic Games, Inc. All Rights Reserved.


#include "WeaponActor.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MyGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Component/HealthComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
AWeaponActor::AWeaponActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	MeshComponent = Cast<UMeshComponent>(FindComponentByClass(UMeshComponent::StaticClass()));
	MeshComponent->SetRenderCustomDepth(true);
		
}


void AWeaponActor::Use(AProtagonistCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	ensure(Character != nullptr);
	AnimInstance = Cast<UProtagonistAnimInstance>(Character->GetMesh()->GetAnimInstance());
	ensure(AnimInstance!=nullptr);

	
	SetupInput();
	SetActorHiddenInGame(false);
}

void AWeaponActor::UnUse()
{
	RemoveInputMappingContext();
	SetActorHiddenInGame(true);
}

void AWeaponActor::SetupInput()
{
	// Set up action bindings
	const APlayerController* const PlayerController = Cast<APlayerController>(Character->GetController());
	ensure(PlayerController != nullptr);
	if (PlayerController == nullptr)
		return;

	AddInputMappingContext(PlayerController);

	if (IsBindInputAction == false)
	{
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
		ensure(EnhancedInputComponent != nullptr);
		BindInputActions(EnhancedInputComponent);
		IsBindInputAction = true;
	}
}

void AWeaponActor::AddInputMappingContext(const APlayerController* PlayerController)
{
	Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	ensure(Subsystem != nullptr);
	if (Subsystem->HasMappingContext(InputMappingContext) == false)
	{
		Subsystem->AddMappingContext(InputMappingContext, InputPriority);
	}
}

void AWeaponActor::RemoveInputMappingContext()
{
	if (Subsystem != nullptr)
	{
		Subsystem->RemoveMappingContext(InputMappingContext);
	}
}
