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
	ReusableCnt = ReusableMaxCnt;
}


void AWeaponActor::OnAttackInputStarted()
{
	if (IsCharging)
		return;

	IsAttack = true;
	AttackInputStarted();

	AttackTriggerIfPossible(ETriggerEvent::Started);
}

void AWeaponActor::OnAttackInputTriggered()
{
	if (!IsAttack)
		return;

	AttackInputTrigger();

	AttackTriggerIfPossible(ETriggerEvent::Triggered);
}

void AWeaponActor::OnAttackInputCompleted()
{
	if (!IsAttack)
		return;

	AttackInputCompleted();

	IsAttack = false;
	AttackTriggerIfPossible(ETriggerEvent::Completed);
}

void AWeaponActor::AttackTriggerIfPossible(ETriggerEvent TriggerEvent)
{
	if (GetAttackTriggerEvent() == TriggerEvent)
	{
		if(ReusableMaxCnt >0)
		{
			ReusableCnt--;
			Protagonist->WeaponComponent->OnUseWeapon.Broadcast(ReusableCnt, ReusableMaxCnt);
		}
		CooldownIfPossible(TriggerEvent);
	}
}

void AWeaponActor::CooldownIfPossible(ETriggerEvent TriggerEvent)
{
	if (CoolTime <= 0)
		return;

	if (ReusableCnt == 0)
	{
		IsCharging = true;
		double CurrentSeconds = GetWorld()->GetTimeSeconds();
		Protagonist->WeaponComponent->OnCooldownWeapon.Broadcast(CurrentSeconds, CurrentSeconds + CoolTime);
		Protagonist->GetWorldTimerManager().SetTimer(RefillTimerHandle, this, &AWeaponActor::OnRefill, CoolTime, false);
	}
}


void AWeaponActor::OnRefill()
{
	IsCharging = false;
	ReusableCnt = ReusableMaxCnt;
}


void AWeaponActor::Equip(AProtagonistCharacter* TargetCharacter)
{
	Protagonist = TargetCharacter;
	ensure(Protagonist != nullptr);
	AnimInstance = Cast<UProtagonistAnimInstance>(Protagonist->GetMesh()->GetAnimInstance());
	ensure(AnimInstance!=nullptr);

	SetupInput();
	SetActorHiddenInGame(false);
}


void AWeaponActor::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &AWeaponActor::OnAttackInputStarted);
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Triggered, this, &AWeaponActor::OnAttackInputTriggered);
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Canceled, this, &AWeaponActor::OnAttackInputCompleted);
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Completed, this, &AWeaponActor::OnAttackInputCompleted);
}


void AWeaponActor::UnEquip()
{
	RemoveInputMappingContext();
	SetActorHiddenInGame(true);
}

void AWeaponActor::SetupInput()
{
	// Set up action bindings
	const APlayerController* const PlayerController = Cast<APlayerController>(Protagonist->GetController());
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
