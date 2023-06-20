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
	
	
};

template <typename T>
T* UtilPlayground::LoadAsset(FString FileName)
{
	static ConstructorHelpers::FObjectFinder<T> MyAsset((TEXT("%s"), *FileName));
	ensure(MyAsset.Succeeded());
	return MyAsset.Object;
}


