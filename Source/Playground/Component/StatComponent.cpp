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


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	CharacterData = GameInstance->GetProtagonistData(StatType, "Default").Get(CharacterData);
	UE_LOG(LogTemp, Log, TEXT("stat hp : %d"), CharacterData.Hp);

}


void UStatComponent::InitializeComponent()
{
	Super::InitializeComponent();



}
