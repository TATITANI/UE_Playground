// Fill out your copyright notice in the Description page of Project Settings.


#include "Outliner/OutlinerSelectionColumn.h"

#include "ActorTreeItem.h"
#include "DebugHeader.h"
#include "ISceneOutlinerTreeItem.h"
#include "TatiEditor.h"
#include "CustomStyle/TatiEditorStyle.h"
#include "Outliner/OutlinerExtension.h"

// 아웃라이더 상단 카테고리
SHeaderRow::FColumn::FArguments FOutlinerSelectionColumn::ConstructHeaderRowColumn()
{
	DebugHeader::PrintLog(TEXT("ConstructHeaderRowColumn"));
	SHeaderRow::FColumn::FArguments ConstructedHeaderRow =
		SHeaderRow::Column(GetColumnID())
		.FixedWidth(24.f)
		.HAlignHeader(HAlign_Center)
		.VAlignHeader(VAlign_Center)
		.HAlignCell(HAlign_Center)
		.VAlignCell(VAlign_Center)
		.DefaultTooltip(FText::FromString(TEXT("Actor Select Lock - Press icon to lock actor selection")))
		[
			SNew(SImage)
		.ColorAndOpacity(FSlateColor::UseForeground())
		.Image(FTatiEditorStyle::GetCreatedSlateSyleSet()->GetBrush(FName("LevelEditor.SelectionLock")))
		];

	return ConstructedHeaderRow;
}

// 아웃라이너 항목 체크박스
const TSharedRef<SWidget> FOutlinerSelectionColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem,
                                                                       const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	FActorTreeItem* ActorTreeItem = TreeItem->CastTo<FActorTreeItem>();
	if (ActorTreeItem == nullptr || ActorTreeItem->IsValid() == false)
	{
		return SNullWidget::NullWidget;
	}

	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));
	const bool IsActorSelected = TatiEditorModule.CheckIsActorSelectionLocked(ActorTreeItem->Actor.Get());


	const FCheckBoxStyle& CheckBoxStyle =
		FTatiEditorStyle::GetCreatedSlateSyleSet()->GetWidgetStyle<FCheckBoxStyle>(FName("SceneOutliner.SelectionLock"));
	
	TSharedRef<SCheckBox> ConstructedRowWidget = SNew(SCheckBox)
	.Visibility(EVisibility::Visible)
	.Type(ESlateCheckBoxType::ToggleButton)
	.Style(&CheckBoxStyle)
	.HAlign(HAlign_Center)
	.IsChecked(IsActorSelected ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
	.OnCheckStateChanged(this, &FOutlinerSelectionColumn::OnRowWidgetCheckStateChanged, ActorTreeItem->Actor);


	return ConstructedRowWidget;
}

void FOutlinerSelectionColumn::OnRowWidgetCheckStateChanged(ECheckBoxState CheckBoxState, TWeakObjectPtr<AActor> TargetActor)
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));

	switch (CheckBoxState)
	{
	case ECheckBoxState::Unchecked:
		TatiEditorModule.OutlinerExtension->ProcessLockingOutliner(TargetActor.Get(), false);
		break;
	case ECheckBoxState::Checked:
		TatiEditorModule.OutlinerExtension->ProcessLockingOutliner(TargetActor.Get(), true);
		break;
	}
}
