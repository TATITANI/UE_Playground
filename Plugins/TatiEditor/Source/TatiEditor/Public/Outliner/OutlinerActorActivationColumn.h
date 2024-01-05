// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISceneOutlinerColumn.h"
#include "TatiEditor.h"

/**
 *  
 */
class TATIEDITOR_API FOutlinerActorActivationColumn : public ISceneOutlinerColumn
{
public:
	FOutlinerActorActivationColumn(ISceneOutliner& SceneOutliner)
	{
		
	};

	virtual FName GetColumnID() override { return FName("ActorActivation"); }
	static FName GetID() { return FName("ActorActivation"); }

	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;
	virtual const TSharedRef<SWidget>
	ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row) override;

private:
	void OnRowWidgetCheckStateChanged(ECheckBoxState CheckBoxState, TWeakObjectPtr<AActor> Actor);
};
