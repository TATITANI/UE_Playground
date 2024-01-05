// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISceneOutlinerColumn.h"

/**
 * 
 */
class TATIEDITOR_API FOutlinerSelectionLockColumn : public ISceneOutlinerColumn
{
public:
	FOutlinerSelectionLockColumn(ISceneOutliner& SceneOutliner)
	{
	}

	virtual FName GetColumnID() override {return FName("SelectionLock");}
	static FName GetID() {return FName("SelectionLock");}
	
	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;
	virtual const TSharedRef<SWidget> ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row) override;

private:
	void OnRowWidgetCheckStateChanged(ECheckBoxState CheckBoxState, TWeakObjectPtr<AActor> TargetActor);

	void ProcessLockingOutliner(AActor* ActorToProcess, bool bShoudLock);

};
