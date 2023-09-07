// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BotGenerator.generated.h"

UCLASS()
class PLAYGROUND_API ABotGenerator : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABotGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	TSubclassOf<class ABotCharacter> SubclassOfBot;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess, MakeEditWidget))
	TArray<FVector> ArrSpawnLoc;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 SpawnCapacity = 10;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	float SpawnCycle = 5;
	
	TArray<ABotCharacter*> Pool;

	FTimerHandle SpawnTimerHandler;
private:
	void SetActiveBot(ABotCharacter *Bot, bool IsActive);

	void TakeBot();

public:
	void ReturnBot(ABotCharacter* Bot);

};
