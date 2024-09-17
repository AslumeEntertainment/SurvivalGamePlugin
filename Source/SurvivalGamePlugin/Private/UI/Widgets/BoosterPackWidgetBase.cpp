// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/BoosterPackWidgetBase.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UBoosterPackWidgetBase::NativeOnInitialized()
{

	Super::NativeOnInitialized();
	BTN_Icon->OnClicked.AddDynamic(this, &UBoosterPackWidgetBase::OnBodyButtonClicked);
}

void UBoosterPackWidgetBase::Config(FText Name, UTexture2D* Image, uint16 Cost)
{

	TXT_Name->SetText(Name);
	TXT_Cost->SetText(FText::FromString(FString::FromInt(Cost)));
	IMG_Icon->SetBrushFromTexture(Image);
}

void UBoosterPackWidgetBase::OnBodyButtonClicked()
{
	OnBodyButton.Broadcast(this);
}

