// Fill out your copyright notice in the Description page of Project Settings.


#include "Outliner/OutlinerActorActivationColumn.h"

#include "ActorTreeItem.h"
#include "DebugHeader.h"
#include "ISceneOutlinerTreeItem.h"
#include "TatiEditorOutlinerSave.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Outliner/OutlinerExtension.h"


SHeaderRow::FColumn::FArguments FOutlinerActorActivationColumn::ConstructHeaderRowColumn()
{
	SHeaderRow::FColumn::FArguments ConstructedHeaderRow =
		SHeaderRow::Column(GetColumnID())
		.FixedWidth(24.f)
		.HAlignHeader(HAlign_Center)
		.VAlignHeader(VAlign_Center)
		.HAlignCell(HAlign_Center)
		.VAlignCell(VAlign_Center)
		.DefaultTooltip(FText::FromString(TEXT("Actor Activation")));

	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));
	if (TatiEditorModule.GetEditorActorSubsystem())
	{
		auto AllActors = TatiEditorModule.WeakEditorActorSubsytem->GetAllLevelActors();
		DebugHeader::PrintLog(FString::Printf(TEXT("AllActors num test :  %d"), AllActors.Num()));
	}

	return ConstructedHeaderRow;
}

const TSharedRef<SWidget> FOutlinerActorActivationColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem,
                                                                             const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	FActorTreeItem* ActorTreeItem = TreeItem->CastTo<FActorTreeItem>();
	if (ActorTreeItem == nullptr || ActorTreeItem->IsValid() == false)
	{
		return SNullWidget::NullWidget;
	}

	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));
	const bool bActive = TatiEditorModule.OutlinerExtension->SavingData->OutlinerActorActivationMap.FindOrAdd(ActorTreeItem->Actor->GetUniqueID(), true);
	const ECheckBoxState CheckBoxState = bActive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	

	// DebugHeader::PrintLog(FString::Printf(TEXT("Outline ConstructRowWidget - key : %u, bool : %d"),
	//                                       ActorTreeItem->Actor->GetUniqueID(), bActive));

	OnRowWidgetCheckStateChanged(CheckBoxState, ActorTreeItem->Actor);

	TSharedRef<SCheckBox> ConstructedRowWidget = SNew(SCheckBox)
	.Visibility(EVisibility::Visible)
	.HAlign(HAlign_Center)
	.IsChecked(CheckBoxState)
	.OnCheckStateChanged(this, &FOutlinerActorActivationColumn::OnRowWidgetCheckStateChanged, ActorTreeItem->Actor);

	return ConstructedRowWidget;
}

void FOutlinerActorActivationColumn::OnRowWidgetCheckStateChanged(ECheckBoxState CheckBoxState, TWeakObjectPtr<AActor> Actor)
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));
	TArray<AActor*> AllActorsInLevel = TatiEditorModule.WeakEditorActorSubsytem->GetAllLevelActors();
	
	const bool bActive = (CheckBoxState == ECheckBoxState::Checked);


	TatiEditorModule.OutlinerExtension->SavingData->OutlinerActorActivationMap.FindOrAdd(Actor->GetUniqueID()) = bActive;

	bool IsSaveSuccess = UGameplayStatics::SaveGameToSlot(TatiEditorModule.OutlinerExtension->SavingData, TEXT("TatiOutliner"), 0);
	// DebugHeader::PrintLog(TEXT("Outliner Active : ") + UEnum::GetValueAsString(CheckBoxState));
	

	Actor->SetIsTemporarilyHiddenInEditor(CheckBoxState == ECheckBoxState::Unchecked);
	Actor->SetActorHiddenInGame(CheckBoxState == ECheckBoxState::Unchecked);
	Actor->SetActorEnableCollision(CheckBoxState == ECheckBoxState::Checked);
	Actor->SetActorTickEnabled(CheckBoxState == ECheckBoxState::Checked);
}
