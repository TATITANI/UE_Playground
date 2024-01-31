// Fill out your copyright notice in the Description page of Project Settings.


#include "BotCharacter.h"

#include "BotAnimInstance.h"
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
#include "Item/DroppedItemTable.h"


// Sets default values
ABotCharacter::ABotCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

// Called when the game starts or when spawned
void ABotCharacter::BeginPlay()
{
	Super::BeginPlay();

	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensure(GameInstance != nullptr);
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
	OnAttackEnd = AnimInstance->OnAttackEnded;
	ensure(OnAttackEnd != nullptr);

	BindUI();
}


// Called every frame
void ABotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABotCharacter::Init(ABotGenerator* _Generator, FVector Loc)
{
	Generator = _Generator;
	HealthComponent->Reset();
	SetActorLocation(Loc);
	ABotAIController* AiController = Cast<ABotAIController>(GetController());
	AiController->ActiveBehaviorTree(true);
}


void ABotCharacter::Attack()
{
	AnimInstance->PlayAttackMontage();
}


void ABotCharacter::BindUI()
{
	UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(GetComponentByClass(UWidgetComponent::StaticClass()));
	WidgetComponent->InitWidget();
	ensure(WidgetComponent != nullptr);
	auto* BotWidget = Cast<UBotWidget>(WidgetComponent->GetWidget());
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

	const FVector TraceRelatviePos = GetActorForwardVector() * attackDistance;
	// 트레이스 채널을 사용해 충돌 감지
	GetWorld()->SweepSingleByChannel(
		OUT hitResult,
		GetActorLocation(),
		GetActorLocation() + TraceRelatviePos,
		FQuat::Identity,
		AttackCollisionChannel,
		FCollisionShape::MakeSphere(attackRadius),
		params);

	AActor* ActorHit = hitResult.GetActor();
	bool IsHit = ActorHit != nullptr && ActorHit->IsA(AProtagonistCharacter::StaticClass());

	FColor ColorDebugCapsule = IsHit ? FColor::Orange : FColor::Green;
	const FQuat QuatDebugCapsule = FRotationMatrix::MakeFromZ(TraceRelatviePos).ToQuat();
	// DrawDebugCapsule(GetWorld(), GetActorLocation() + TraceRelatviePos * 0.5f,
	// attackDistance * 0.5f, attackRadius, QuatDebugCapsule, ColorDebugCapsule, false, 2.f);

	if (IsHit)
	{
		UGameplayStatics::ApplyDamage(ActorHit, AttackDamage, this->GetController(),
		                              this->GetOwner(), UDamageType::StaticClass());
	}
}

void ABotCharacter::OnDeadCallback()
{
	Generator->ReturnBot(this);
	ABotAIController* AiController = this->GetController<ABotAIController>();
	AiController->ActiveBehaviorTree(false);

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
	AnimInstance->PlayAttackedMontage();
}

bool ABotCharacter::IsAttacked()
{
	return AnimInstance->IsPlayingAttackedMontage();
}
