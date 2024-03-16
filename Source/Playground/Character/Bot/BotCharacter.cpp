// Fill out your copyright notice in the Description page of Project Settings.


#include "BotCharacter.h"

#include "BotAnimInstance.h"
#include "DamageType_KnockOut.h"
#include "KismetAnimationLibrary.h"
#include "MyGameInstance.h"
#include "AI/BotAIController.h"
#include "AI/BotGenerator.h"
#include "UI/BotWidget.h"
#include "Component/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/PlaygroundItem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogMacros.h"
#include "PlaygroundGameMode.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/DroppedItemTable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/LogMacros.h"
#include "PhysicsEngine/ConstraintUtils.h"
#include "UI/FloatingDamage.h"
#include "Utils/UtilPlayground.h"


// Sets default values
ABotCharacter::ABotCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve p0erformance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	HpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hp Widget"));
	HpWidgetComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ABotCharacter::BeginPlay()
{
	Super::BeginPlay();

	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensure(GameInstance != nullptr);

	AIController = Cast<ABotAIController>(GetController());
	ensure(AIController);
	
	auto BotStat = GameInstance->GetCharacterStat<FBotStat>
		(ECharacterStatType::Bot, "1");
	
	HealthComponent->Init(BotStat->MaxHp);
	HealthComponent->OnDead.AddUObject(this, &ABotCharacter::OnDeadCallback);
	OnTakeAnyDamage.AddDynamic(this, &ABotCharacter::OnTakeDamageCallback);

	AttackDamage = BotStat->Damage;
}

void ABotCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UBotAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(AnimInstance != nullptr, TEXT("Bot animinstance cast failed"));
	AnimInstance->OnAttackHit.AddUObject(this, &ABotCharacter::CheckAttack);

	BindUI();
}


// Called every frame
void ABotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector HpLookDir = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation() - GetActorLocation();
	HpLookDir.Z = 0;
	const FRotator HpRotation = UKismetMathLibrary::MakeRotFromX(HpLookDir);
	HpWidgetComponent->SetWorldRotation(HpRotation);

	CurrentVelocityAngle = (AIController->GetFocusActor() == nullptr) ? 0 : UKismetAnimationLibrary::CalculateDirection(GetVelocity(), GetActorRotation());
}

void ABotCharacter::Init(ABotGenerator* _Generator, FVector Loc)
{
	Generator = _Generator;
	HealthComponent->Reset();
	SetActorLocation(Loc);
	AIController->StartBehaviorTreeLogic();
	SetState(EBotState::Idle);
}


void ABotCharacter::Attack()
{
	SetState(EBotState::Attacking);
	AnimInstance->PlayAttackMontage(GetCurrentState());
}


void ABotCharacter::SetState(EBotState::Type BotState)
{
	CurrentBotState = BotState;
	// UtilPlayground::PrintLog(FString::Printf(TEXT("Bot SetState : %s"), *UEnum::GetValueAsString(BotState)));
}

bool ABotCharacter::ResetCurrentState(EBotState::Type ResetConditionState)
{
	if (CurrentBotState == ResetConditionState)
	{
		SetState(EBotState::Idle);
		return true;
	}
	
	return false;
}

void ABotCharacter::BindUI()
{
	HpWidgetComponent->InitWidget();
	ensure(HpWidgetComponent != nullptr);

	auto* BotWidget = Cast<UBotWidget>(HpWidgetComponent->GetWidget());
	ensureMsgf(BotWidget != nullptr, TEXT("Bot Widget not found"));
	BotWidget->Bind(HealthComponent);
}

/**
 * @brief 판정 후 데미지 적용
 */
void ABotCharacter::CheckAttack()
{
	FHitResult hitResult;
	FCollisionQueryParams params(NAME_None, false, this);

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * AttackDistance,
	                                        AttackRadius, AttackTraceQuery, false, {},
	                                        EDrawDebugTrace::ForDuration, hitResult, true);


	AActor* ActorHit = hitResult.GetActor();
	const bool IsHit = ActorHit != nullptr && ActorHit->IsA(AProtagonistCharacter::StaticClass());
	if (IsHit)
	{
		UGameplayStatics::ApplyDamage(ActorHit, AttackDamage, this->GetController(),
		                              this->GetOwner(), UDamageType::StaticClass());
	}
}

void ABotCharacter::OnDeadCallback()
{
	Generator->ReturnBot(this);
	AIController->StopBehaviorTree(FString("Bot Dead"));

	// 드랍 아이템
	{
		const APlaygroundGameMode* PlaygroundGameMode = Cast<APlaygroundGameMode>(GetWorld()->GetAuthGameMode());
		const auto Data = PlaygroundGameMode->DroppedItemTable->GetDroppedItemData();
		const UItemData* ItemData = Data.Key;
		const FItemStatus ItemStatus = Data.Value;

		const auto DroppedItem = GetWorld()->SpawnActor<APlaygroundItem>(ItemData->DroppedItem);
		DroppedItem->Init(GetActorLocation(), ItemStatus);
	}
}

void ABotCharacter::OnTakeDamageCallback(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
                                         AActor* DamageCauser)
{
	if(GetCurrentState() == EBotState::KnockOut)
		return;
	
	if (DamageType->IsA(UDamageType_KnockOut::StaticClass()))
	{
		KnockOut();
	}
	else
	{
		SetState(EBotState::Attacked);
		AnimInstance->PlayAttackedMontage(GetCurrentState());
	}
}

void ABotCharacter::KnockOut()
{
	SetState(EBotState::KnockOut);
	AnimInstance->StopAllMontages(0);
	
	GetWorldTimerManager().SetTimer(KnockOutTimerHandle, FTimerDelegate::CreateLambda([this]
	{
		SetState(EBotState::StandUp);
	}), KnockOutDuration, false);
}
