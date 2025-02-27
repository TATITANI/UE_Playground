// Copyright Epic Games, Inc. All Rights Reserved.


#include "WeaponActor.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MyGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Component/CharacterWeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Utils/UtilPlayground.h"


// Sets default values for this component's properties
AWeaponActor::AWeaponActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	bReplicates = true;
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent = Cast<UMeshComponent>(FindComponentByClass(UMeshComponent::StaticClass()));
	MeshComponent->SetRenderCustomDepth(true);

	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensureAlways(GameInstance != nullptr);
	auto WeaponStat = GameInstance->GetWeaponStat<FWeaponStat>(GetWeaponType(), FName("1"));

	ensureAlwaysMsgf(WeaponStat.IsSet(), TEXT("Weapon Stat is Null"));
	this->Damage = WeaponStat->Damage;
	this->CoolTime = WeaponStat->CoolTime;
	ReusableCnt = ReusableMaxCnt;
}

void AWeaponActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
		if (ReusableMaxCnt > 0)
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

void AWeaponActor::ServerSetProtagonist_Implementation(AProtagonistCharacter* InProtagonist)
{
	SetProtagonist(InProtagonist);
	PG_LOG(LogPGNetwork,Log,TEXT("Pro null :%d"),(Protagonist == nullptr));
}


void AWeaponActor::SetProtagonist(AProtagonistCharacter* InProtagonist)
{
	this->Protagonist = InProtagonist;
	this->ProtagonistAnimInstance = Cast<UProtagonistAnimInstance>(Protagonist->GetMesh()->GetAnimInstance());

}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponActor, Protagonist);
}

void AWeaponActor::OnRefill()
{
	IsCharging = false;
	ReusableCnt = ReusableMaxCnt;
}


void AWeaponActor::Equip(AProtagonistCharacter* TargetCharacter)
{
	PG_LOG(LogPGNetwork, Log, TEXT(""));

	if (ensureAlwaysMsgf(TargetCharacter, TEXT("Protagonist nullptr")) == false)
	{
		return;
	}

	SetProtagonist(TargetCharacter);
	if (TargetCharacter->IsLocallyControlled())
	{
		ServerSetProtagonist(TargetCharacter);
	}

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
	if (ensureAlwaysMsgf(Protagonist != nullptr, TEXT("Protagonist Null")) == false)
	{
		return;
	}

	if (Protagonist->IsLocallyControlled() == false)
		return;

	// Set up action bindings
	const APlayerController* const PlayerController = Cast<APlayerController>(Protagonist->GetController());
	ensureAlways(PlayerController != nullptr);
	if (PlayerController == nullptr)
	{
		PG_LOG(LogTemp, Log, TEXT("PlayerController is nullptr"));
		return;
	}


	AddInputMappingContext(PlayerController);

	if (IsBindInputAction == false)
	{
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
		if (ensureAlwaysMsgf(EnhancedInputComponent != nullptr, TEXT("EnhancedInputComponent is nullptr")))
		{
			PG_LOG(LogPGNetwork, Log, TEXT("Bind Input"));
			BindInputActions(EnhancedInputComponent);
			BindInputActionsImpl(EnhancedInputComponent);

			IsBindInputAction = true;
		}
	}
}

void AWeaponActor::AddInputMappingContext(const APlayerController* PlayerController)
{
	Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	ensureAlways(Subsystem != nullptr);
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
