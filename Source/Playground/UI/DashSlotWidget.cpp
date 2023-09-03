// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DashSlotWidget.h"

#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Component/DashComponent.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UDashSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Img_Panel->Brush.GetResourceObject());
	MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MaterialInterface, nullptr);
	MaterialInstanceDynamic->AddToRoot(); // GC 삭제 방지
	Img_Panel->SetBrushFromMaterial(MaterialInstanceDynamic);

	const auto PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	const auto ProtagonistCharacter = Cast<AProtagonistCharacter>(PlayerPawn);
	ensure(ProtagonistCharacter!= nullptr);
	ProtagonistCharacter->DashComponent->OnCooldownDash.AddUObject(this, &UDashSlotWidget::ActiveCooldown);
}

void UDashSlotWidget::ActiveCooldown(double _CoolBeginSeconds, double _CoolEndSeconds)
{
	IsCooldown = true;
	CoolBeginSeconds = _CoolBeginSeconds;
	CoolEndSeconds = _CoolEndSeconds;
}


void UDashSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsCooldown)
	{
		double CurrentSeconds = GetWorld()->GetTimeSeconds();
		float Progress = FMath::Clamp((CurrentSeconds - CoolBeginSeconds) / (CoolEndSeconds - CoolBeginSeconds), 0, 1);

		MaterialInstanceDynamic->SetScalarParameterValue(KeyMaterialProgress, Progress);
		if (CurrentSeconds > CoolEndSeconds)
		{
			IsCooldown = false;
			MaterialInstanceDynamic->SetScalarParameterValue(KeyMaterialProgress, 0);
		}
	}
}
