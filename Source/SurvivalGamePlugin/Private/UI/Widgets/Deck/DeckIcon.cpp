// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/DeckIcon.h"

#include "Kismet/KismetStringLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UDeckIcon::OnIconClicked()
{
	IconClicked.Broadcast(UKismetStringLibrary::Conv_StringToInt(TXT_DeckNumber->GetText().ToString()));
}

void UDeckIcon::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Icon->OnClicked.AddDynamic(this, &UDeckIcon::OnIconClicked);
}

void UDeckIcon::SetIsEnabled(bool NewValue)
{
	BTN_Icon->SetIsEnabled(NewValue);
}

void UDeckIcon::SetDeckNumber(int DeckNumber)
{
	TXT_DeckNumber->SetText(FText::FromString(FString::FromInt(DeckNumber)));
}

int UDeckIcon::GetDeckNumber()
{
	return UKismetStringLibrary::Conv_StringToInt(TXT_DeckNumber->GetText().ToString());
}
