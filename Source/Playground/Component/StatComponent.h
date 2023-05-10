// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterData.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

UENUM(BlueprintType)
enum class ECharacterStatType : uint8
{
	Protagonist UMETA(DisplayName="Protagonist") ,
	Bot UMETA(displayDisplayName = "Bot"),
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void InitializeComponent() override;

private:
	UPROPERTY(EditAnywhere, Category=Stat, meta=(AllowPrivateAccess=true))
	ECharacterStatType StatType;

	UPROPERTY(VisibleAnywhere, Category=Stat, meta=(AllowPrivateAccess=true))
	FCharacterData CharacterData;
};
