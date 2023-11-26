// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/AdvanceDeletionWidget.h"

#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "TatiEditor.h"
#include "Widgets/Layout/SScrollBox.h"

#define ListAll TEXT("List all available assets")
#define ListUnused TEXT("List unused Assets")
#define ListSameName TEXT("List Same Name Assets")

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	UE_LOG(LogTemp, Log, TEXT("Construct"));
	bCanSupportFocus = true;

	this->StoredAssetDatas = InArgs._AssetDatas;
	this->DisplayedAssetsData = StoredAssetDatas;

	this->SelectedDatas.Empty();
	this->CheckBoxes.Empty();
	ComboSourceItems.Empty();


	ComboSourceItems.Add(MakeShared<FString>(ListAll));
	ComboSourceItems.Add(MakeShared<FString>(ListUnused));
	ComboSourceItems.Add(MakeShared<FString>(ListSameName));

	FSlateFontInfo TitleTextFont = GetEmbosedTextFont();
	TitleTextFont.Size = 30;


	ChildSlot
	[
		SNew(SVerticalBox)

		// 첫번째 vertical box slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				ConstructComboBox()
			]
		]

		// 카테고리
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			ConstructCategory()
		]

		// 에셋 리스트
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill) // 스크롤바 적용
		[
			SNew(SScrollBox)

			+ SScrollBox::Slot()
			[
				ConstructAssetList()
			]
		]

		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			  .FillWidth(10.f)
			  .Padding(5.f)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Select/DeSelect All")))
				.OnClicked(this, &SAdvanceDeletionTab::OnSelectAndDeSelectAllBtnClicked)
			]

			+ SHorizontalBox::Slot()
			  .FillWidth(10.f)
			  .Padding(5.f)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Delete Selected Items")))
				.OnClicked(this, &SAdvanceDeletionTab::OnDeleteSelectingBtnClicked)
			]

		]

	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetList()
{
	ConstructedAssetList = SNew(SListView<TSharedPtr<FAssetData>>)
			.ItemHeight(16)
			.ListItemsSource(&DisplayedAssetsData)
			.OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRowForList)
	.OnMouseButtonClick(this, &SAdvanceDeletionTab::OnRowWidgetMouseBtnClicked);

	return ConstructedAssetList.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
	CheckBoxes.Empty();
	SelectedDatas.Empty();

	if (ConstructedAssetList.IsValid())
	{
		// OnGenerateRow, 데이터 등 다시 갱신됨
		ConstructedAssetList->RebuildList();
	}
}

TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConstructComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox =
		SNew(SComboBox< TSharedPtr<FString>>)
	.OptionsSource(&ComboSourceItems)
	.OnGenerateWidget(this, &SAdvanceDeletionTab::OnGenerateComboContent)
	.OnSelectionChanged(this, &SAdvanceDeletionTab::OnComboSelectionChanged)
		[
			// 콤보박스 선택 영역
			SAssignNew(ComboDisplayTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("List Assets Option")))
		];


	return ConstructedComboBox;
}

#pragma region RowWidgetForAssetlistView

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> ComboText = SNew(STextBlock).Text(FText::FromString(*SourceItem.Get()));

	return ComboText;
}

void SAdvanceDeletionTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type SelectInfo)
{
	DebugHeader::Print(*SelectedOption.Get(), FColor::Emerald);
	ComboDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));

	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));


	if (*SelectedOption.Get() == ListAll)
	{
		DisplayedAssetsData = StoredAssetDatas;
	}
	else if (*SelectedOption.Get() == ListUnused)
	{
		TatiEditorModule.ListUnusedAssets(StoredAssetDatas, DisplayedAssetsData);
	}
	else if (*SelectedOption.Get() == ListSameName)
	{
		TatiEditorModule.ListSameNamedAssets(StoredAssetDatas, DisplayedAssetsData);
	}
	RefreshAssetListView();
}

TSharedRef<SHorizontalBox> SAdvanceDeletionTab::ConstructCategory()
{
	return SNew(SHorizontalBox)

	+ SHorizontalBox::Slot()
	  .HAlign(HAlign_Center)
	  .MaxWidth(250.f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Class")))
	]

	+ SHorizontalBox::Slot()
	  .HAlign(HAlign_Center)
	  .MaxWidth(900.f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Directory")))
	]

	+ SHorizontalBox::Slot()
	  .HAlign(HAlign_Center)
	  .MaxWidth(200.f)

	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("AssetName")))
	];
	
}


/**
 * @brief verticalBox에 표시되는 항목 
 */
TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,
                                                                const TSharedRef<STableViewBase>& OwnerTable)
{
	if (AssetDataToDisplay.IsValid() == false)
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);

	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	FSlateFontInfo AssetClassNameFont = GetEmbosedTextFont();
	AssetClassNameFont.Size = 10;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(5.f))

		[
			SNew(SHorizontalBox)

			// 체크박스
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Center)
			  .FillWidth(0.1f)
			[
				ConstructCheckBox(AssetDataToDisplay)
			]

			// 클래스 이름
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Fill)
			  .FillWidth(1.f)
			[
				ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
			]

			// 경로
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Fill)
			  .FillWidth(2.f)
			[
				SNew(STextBlock)
				.Font(GetEmbosedTextFont())
				.ColorAndOpacity(FColor::White)
				.Text(FText::FromString(AssetDataToDisplay->PackagePath.ToString()))
			]

			// 에셋 이름
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Fill)
			[
				ConstructTextForRowWidget(DisplayAssetName, AssetClassNameFont)
			]

			//
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Fill)
			[
				ConstructButtonForRowWidget(AssetDataToDisplay)
			]


		];

	return ListRowWidget;
}

/**
 * @brief
 *  콘텐츠브라우저를 클릭한 에셋의 위치로 이동  
 */
void SAdvanceDeletionTab::OnRowWidgetMouseBtnClicked(TSharedPtr<FAssetData> ClickedData)
{
	DebugHeader::Print(ClickedData->AssetName.ToString() + TEXT("is clicked"), FColor::Blue);

	UEditorAssetLibrary::SyncBrowserToObjects({ClickedData->GetObjectPathString()});
	
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructedCheckBox = SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox)
	.OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
	.Visibility(EVisibility::Visible);
	
	
	CheckBoxes.AddUnique(ConstructedCheckBox);

	return ConstructedCheckBox;
}


TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& Font)
{
	TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
	.Text(FText::FromString(TextContent))
	.Font(Font)
	.ColorAndOpacity(FColor::White);

	return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> ConstructedBtn = SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SAdvanceDeletionTab::OnDeleteButtonClicked, AssetDataToDisplay);


	return ConstructedBtn;
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));
	const bool bDelete = TatiEditorModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());

	if (bDelete)
	{
		if (StoredAssetDatas.Contains(ClickedAssetData))
		{
			StoredAssetDatas.Remove(ClickedAssetData);
		}

		if (DisplayedAssetsData.Contains(ClickedAssetData))
		{
			DisplayedAssetsData.Remove(ClickedAssetData);
		}

		RefreshAssetListView();
	}


	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnDeleteSelectingBtnClicked()
{
	TArray<FAssetData> AssetsToDelete;

	for (auto CheckedData : SelectedDatas)
	{
		AssetsToDelete.Add(*CheckedData.Get());
	}

	const bool bDelete = ObjectTools::DeleteAssets(AssetsToDelete) > 0;
	if (bDelete)
	{
		for (auto DeletedData : SelectedDatas)
		{
			if (StoredAssetDatas.Contains(DeletedData))
			{
				StoredAssetDatas.Remove(DeletedData);
			}

			if (DisplayedAssetsData.Contains(DeletedData))
			{
				DisplayedAssetsData.Remove(DeletedData);
			}
		}

		RefreshAssetListView();
	}

	return FReply::Handled();
}

FReply SAdvanceDeletionTab::OnSelectAndDeSelectAllBtnClicked()
{
	const bool bSelectMode = SelectedDatas.Num() < StoredAssetDatas.Num();

	if (bSelectMode)
	{
		SelectedDatas = StoredAssetDatas;
	}
	else
	{
		SelectedDatas.Empty();
	}


	for (const auto CheckBox : CheckBoxes)
	{
		CheckBox->SetIsChecked(bSelectMode ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}

	return FReply::Handled();
}


void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	// DebugHeader::Print(AssetData->AssetName.ToString(), FColor::Green);
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		if (SelectedDatas.Contains(AssetData))
		{
			SelectedDatas.RemoveSingle(AssetData);
		}
		break;

	case ECheckBoxState::Checked:
		SelectedDatas.AddUnique(AssetData);
		break;

	case ECheckBoxState::Undetermined:
		break;
	}
}


#pragma  endregion
