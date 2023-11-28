// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

class FTatiEditorStyle
{
public:
	static void InitalizeIcons();
	static void Shutdown();

	static FName GetStyleSetName() {return StyleSetName;}
private:
	static FName StyleSetName;

	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();
	static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;
	
};