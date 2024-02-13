// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FloatingDamage.h"

#include "Components/WidgetComponent.h"
#include "Logging/LogMacros.h"

AFloatingDamage::AFloatingDamage()
{
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	SetRootComponent(WidgetComponent);
}

void AFloatingDamage::BeginPlay()
{
	Super::BeginPlay();
}

void AFloatingDamage::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ElapsedTime += DeltaSeconds;
	const float Progress = ElapsedTime / MovingDuration;
	const float ProgressValue = MovingCurve->GetFloatValue(Progress);
	const FVector Location = FMath::Lerp(StartLoc, EndLoc, ProgressValue);
	SetActorLocation(Location);
	if (Progress > 1)
	{
		Destroy();
	}
}

void AFloatingDamage::SetMovingDuration(float _MovingDuration)
{
	MovingDuration = _MovingDuration;
}

void AFloatingDamage::Init(int32 Damage, FVector Loc, FRotator Rot)
{
	StartLoc = Loc;
	EndLoc = StartLoc +
		GetActorRightVector() * FMath::RandRange(EndLocMin.X, EndLocMax.X)
		+ GetActorUpVector() * FMath::RandRange(EndLocMin.Y, EndLocMax.Y);

	SetActorLocation(StartLoc);
	SetActorRotation(Rot);
	SetDamageTxt(Damage);
}
