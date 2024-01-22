// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "Character/Bot/BotCharacter.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}


void UHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GetOwner()->OnTakeAnyDamage.AddUniqueDynamic(this, &UHealthComponent::HandleTakenDamage);
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	AnimInstance = Cast<UAnimInstance>(Character->GetMesh()->GetAnimInstance());

}

void UHealthComponent::Init(int32 _MaxHp)
{
	MaxHp = _MaxHp;
	Reset();
}

void UHealthComponent::Reset()
{
	CurrentHp = MaxHp;
	OnHpChanged.Broadcast(CurrentHp,0, MaxHp);
}


void UHealthComponent::PostInitProperties()
{
	Super::PostInitProperties();
}

void UHealthComponent::HandleTakenDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
                                         AActor* DamageCauser)
{
	CurrentHp = FMath::Max(CurrentHp - Damage, 0);
	OnHpChanged.Broadcast(CurrentHp, -Damage, MaxHp);

	if (AnimInstance)
	{
		if(DamagedMontage)
			AnimInstance->Montage_Play(DamagedMontage);
	}

	if (CurrentHp <= 0)
	{
		OnDead.Broadcast();
	}


	if (DamagedActor != nullptr && DamageCauser != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("OwnerName : %s, DamagedActor : %s, DamageCauser : %s, taken damage hp : %d"),
		       *GetOwner()->GetName(), *DamagedActor->GetName(), *DamageCauser->GetName(), CurrentHp);
	}
}
