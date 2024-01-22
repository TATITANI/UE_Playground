// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PLAYGROUND_API UtilPlayground
{
public:
	UtilPlayground();
	~UtilPlayground();

	template <typename T>
	static T* LoadAsset(FString FileName);

	static void PrintLog(const FString& Message)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}

	static void Print(const FString& Message, const FColor& Color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
			PrintLog(Message);
		}
	}
	
};

template <typename T>
T* UtilPlayground::LoadAsset(FString FileName)
{
	static ConstructorHelpers::FObjectFinder<T> MyAsset((TEXT("%s"), *FileName));
	ensure(MyAsset.Succeeded());
	return MyAsset.Object;
}


