// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MarchingCubeDetailCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "MarchingCubeWorld.h"
#include "SlateOptMacros.h"
#include "ToolSetupUtil.h"

FMarchingCubeDetailCustomization::FMarchingCubeDetailCustomization()
{
}

FMarchingCubeDetailCustomization::~FMarchingCubeDetailCustomization()
{
}

TSharedRef<IDetailCustomization> FMarchingCubeDetailCustomization::MakeInstance()
{
	return MakeShareable(new FMarchingCubeDetailCustomization);
}

// 슬레이트 코드 컴파일 최적화
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FMarchingCubeDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("New MarchingCube",
	                                                              FText::FromString("MarchingCube"),
	                                                              ECategoryPriority::Important);


	// 버튼을 맨 아래에 배치하기 위해 커스텀 카테고리에 프로퍼티를 추가 
	const TSharedRef<IPropertyHandle> PositionProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(AMarchingCubeWorld, Position));
	
	Category.AddProperty(PositionProperty);

	Category.AddCustomRow(FText::GetEmpty())
	        .WholeRowContent()
	        .VAlign(VAlign_Bottom)
	        .HAlign(HAlign_Right)
	[
		SNew(SButton)
			.Text(FText::FromString("Generate"))
			.VAlign(VAlign_Bottom)
			// .OnClicked(this, &FMarchingCubeDetailCustomization::OnClickGenerate)
			.OnClicked_Lambda([&]()
		             {
			             UObject* SelectedObject = DetailBuilder.GetDetailsView()->GetSelectedObjects()[0].Get();
			             AMarchingCubeWorld* SelectedActor = Cast<AMarchingCubeWorld>(SelectedObject);

			             return FReply::Handled();
		             })
	];
}

FReply FMarchingCubeDetailCustomization::OnClickGenerate()
{
	UE_LOG(LogTemp, Log, TEXT("MC Generate Click"));


	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
