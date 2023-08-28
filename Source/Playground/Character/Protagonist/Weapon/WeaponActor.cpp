// Copyright Epic Games, Inc. All Rights Reserved.


#include "WeaponActor.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MyGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Component/CharacterWeaponComponent.h"
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

	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensure(GameInstance != nullptr);
	auto WeaponStat = GameInstance->GetWeaponStat<FWeaponStat>(GetWeaponType(), FName("1"));

	ensureMsgf(WeaponStat.IsSet(), TEXT("Weapon Stat is Null"));
	this->Damage = WeaponStat->Damage;
	this->CoolTime = WeaponStat->CoolTime;
}


void AWeaponActor::OnAttackStarted()
{
	if (IsCharging)
		return;

	IsAttack = true;
	AttackStart();

	CooldownIfPossible(ETriggerEvent::Started);

}

void AWeaponActor::OnAttackTriggered()
{
	if (!IsAttack)
		return;

	AttackTrigger();
	CooldownIfPossible(ETriggerEvent::Triggered);

}

void AWeaponActor::OnAttackCompleted()
{
	if (!IsAttack)
		return;

	AttackFinish();

	IsAttack = false;
	CooldownIfPossible(ETriggerEvent::Completed);
}

void AWeaponActor::CooldownIfPossible(ETriggerEvent TriggerEvent)
{
	if (CoolTime <= 0)
		return;

	if (GetCooldownOccurEvent() == TriggerEvent && CheckCooldown())
	{
		IsCharging = true;
		double CurrentSeconds =  GetWorld()->GetTimeSeconds();
		Character->WeaponComponent->OnCooldown.Broadcast(CurrentSeconds, CurrentSeconds + CoolTime);
		Character->GetWorldTimerManager().SetTimer(RefillTimerHandle, this, &AWeaponActor::OnRefill, CoolTime, false);
	}
}


void AWeaponActor::OnRefill()
{
	IsCharging = false;
}


bool AWeaponActor::CheckCooldown()
{
	return true;
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


void AWeaponActor::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &AWeaponActor::OnAttackStarted);
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Triggered, this, &AWeaponActor::OnAttackTriggered);
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Canceled, this, &AWeaponActor::OnAttackCompleted);
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Completed, this, &AWeaponActor::OnAttackCompleted);
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
