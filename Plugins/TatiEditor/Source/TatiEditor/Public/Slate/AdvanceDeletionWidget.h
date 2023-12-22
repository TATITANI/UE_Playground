// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvanceDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvanceDeletionTab)
		{
		}

		SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetDatas)

	SLATE_END_ARGS()

public:		
	void Construct(const FArguments& InArgs);
	UPROPERTY()
	static const FName AdvanceDeletionName;
	
private:
	TArray<TSharedPtr<FAssetData>> StoredAssetDatas;
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsData;
	TArray<TSharedPtr<FAssetData>> SelectedDatas;
	TArray<TSharedRef<SCheckBox>> CheckBoxes;

	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetList();
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetList;
	void RefreshAssetListView();

#pragma region ComboBoxForListeningCondition

	TSharedPtr<STextBlock> ComboDisplayTextBlock;
	
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	TArray<TSharedPtr<FString>> ComboSourceItems;
	TSharedRef<SWidget> OnGenerateComboContent(TSharedPtr<FString> SourceItem);
	void OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type SelectInfo);

#pragma endregion

	TSharedRef<SHorizontalBox> ConstructCategory();

#pragma  region RowWidgetForAssetlistView

	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,
	                                           const TSharedRef<STableViewBase>& OwnerTable);
	void OnRowWidgetMouseBtnClicked(TSharedPtr<FAssetData> ClickedData);	
	
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent,
	                                                 const FSlateFontInfo& Font);
	TSharedRef<SButton> ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

#pragma  endregion

	FReply OnDeleteSelectingBtnClicked();
	FReply OnSelectAndDeSelectAllBtnClicked();

	FSlateFontInfo GetEmbosedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }
};
