// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TATIEDITOR_API FOutlinerExtension
{
public:
	FOutlinerExtension();
	~FOutlinerExtension();

	
private:
	void RegisterOutlinerColumn();
	TSharedRef<class ISceneOutlinerColumn> OnCreateSelectionLockColumn(class ISceneOutliner& SceneOutliner);
	void UnregisterOutlinerColumExtension();

public:
	
	void ProcessLockingOutliner(AActor* ActorToProcess, bool bShoudLock);

	
};
