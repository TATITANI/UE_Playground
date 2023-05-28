// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

#include "Character/Bot/BotCharacter.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}


void UStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GetOwner()->OnTakeAnyDamage.AddUniqueDynamic(this, &UStatComponent::HandleTakenDamage);
}

// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// init data
	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensure(GameInstance != nullptr);
	CharacterData = GameInstance->GetCharacterData(StatType, "Default").Get(CharacterData);
	CurrentHp = CharacterData.MaxHp;
	OnHpChanged.Broadcast(CurrentHp, CharacterData.MaxHp);
}

void UStatComponent::HandleTakenDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
                                       AActor* DamageCauser)
{
	CurrentHp = FMath::Max(CurrentHp - Damage, 0);
	OnHpChanged.Broadcast(CurrentHp, CharacterData.MaxHp);

	if (DamagedActor != nullptr && DamageCauser != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("OwnerName : %s, DamagedActor : %s, DamageCauser : %s, taken damage hp : %d"),
		       *GetOwner()->GetName(), *DamagedActor->GetName(), *DamageCauser->GetName(), CurrentHp);
	}
}
