// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner of Dash Component was not found"));
		PrimaryComponentTick.bCanEverTick = false;
		return;
	}

	float minTime;
	DashPowerCurve->GetTimeRange(minTime, DashMaxTime);

	auto PawnOwner = Cast<APawn>(Owner);

	if (const APlayerController* PlayerController = Cast<APlayerController>(PawnOwner->GetController()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &UDashComponent::StartDash);
			UE_LOG(LogTemp, Log, TEXT("dash beginplay"));
		}
	}
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DashRemainTime > 0)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White,
		                                 FString::Printf(TEXT("DashRemainTime : %f"), DashRemainTime));
		ProgressDash(DeltaTime);
	}
}


void UDashComponent::StartDash(const FInputActionValue& Value)
{
	if (!CanDash)
		return;

	CanDash = false;

	DashRemainTime = DashMaxTime;
	// auto TimerMgr = Owner->GetWorldTimerManager();
	Owner->GetWorldTimerManager().SetTimer(DashTimerHandle, FTimerDelegate::CreateLambda([this]
	{
		CanDash = true;
		Owner->GetWorldTimerManager().ClearTimer(DashTimerHandle);
		// UE_LOG(LogTemp, Log, TEXT("can dash"));
	}), 2.0f, false);
}

void UDashComponent::ProgressDash(float& dt)
{
	DashRemainTime -= dt;
	const float DashCoefficient = DashPowerCurve->GetFloatValue(DashMaxTime - DashRemainTime);
	// UE_LOG(LogTemp, Log, TEXT("DashRemainTime : %f, DashCoefficient: %f"), DashRemainTime, DashCoefficient);

	Owner->AddActorWorldOffset(Owner->GetActorForwardVector() * DashPower * DashCoefficient * dt);
}
