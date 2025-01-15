// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Lobby/Logistics/LobbyManagingWidgetBase.h"

#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"

#include "UI/Widgets/Core/TextButtonWidgetBase.h"


void ULobbyManagingWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Exit->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnExitButtonClicked);
	BTN_Leave->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnLeaveButtonClicked);
	BTN_ChooseCampaign->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnChooseCampaignClicked);
	BTN_ChangeCampaign->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnChooseCampaignClicked);
	BTN_StartCampaign->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnStartCampaignClicked);
	BTN_StopCampaign->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnStopCampaignClicked);
	BTN_Supervised->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnSupervisedClicked);
	BTN_Unsupervised->OnClicked.AddDynamic(this, &ULobbyManagingWidgetBase::OnUnsupervisedClicked);
}

void ULobbyManagingWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	WSW_CampaignWindow->SetActiveWidgetIndex(CurrentActiveWidget);
}

void ULobbyManagingWidgetBase::MarkForDestruction()
{
	RemoveFromParent();
	Destruct();
	MarkAsGarbage();
}

void ULobbyManagingWidgetBase::SetSupervised(bool State)
{
	TXT_Supervised->SetText(FText::FromString(State ? "Supervised" : "Unsupervised"));
}

void ULobbyManagingWidgetBase::FillCampaign(FText Name, uint8 PlayerNum, bool IsSupervised, bool IsChangeable, TArray<FString> Games, uint8 Progression, uint8 CurrentGame)
{
	BTN_Supervised->SetIsEnabled(IsChangeable ? !IsSupervised : false);
	BTN_Unsupervised->SetIsEnabled(IsChangeable ? IsSupervised : false);

	BTN_StopCampaign->SetIsEnabled(false);

	TXT_Name->SetText(Name);
	TXT_PlayerNum->SetText(FText::FromString(FString::FromInt(PlayerNum)));
	SetSupervised(IsSupervised);

	SCRB_Games->ClearChildren();
	UTextButtonWidgetBase* GameWidget;
	for (uint8 i=0; i<Games.Num(); i++)
	{
		GameWidget = CreateWidget<UTextButtonWidgetBase>(this, GameWidgetClass);
		GameWidget->OnButtonClickedSelf.AddDynamic(this, &ULobbyManagingWidgetBase::OnGameClicked);

		GameWidget->SetText(FText::FromString(Games[i] + " " + (i <= Progression ? (i == CurrentGame ? CurrentSymbol : CompletedSymbol) : "")));
		GameWidget->SetEnabledStatus(i <= Progression && i!= CurrentGame);

		SCRB_Games->AddChild(GameWidget);

	}

	CurrentActiveWidget = 1;
	WSW_CampaignWindow->SetActiveWidgetIndex(1);
}

void ULobbyManagingWidgetBase::ResetCampaign()
{
	CurrentActiveWidget = 0;
	WSW_CampaignWindow->SetActiveWidgetIndex(0);
}

void ULobbyManagingWidgetBase::OnExitButtonClicked()
{
	OnExitButton.Broadcast();
}

void ULobbyManagingWidgetBase::OnLeaveButtonClicked()
{
	OnLeaveButton.Broadcast();
}

void ULobbyManagingWidgetBase::OnChooseCampaignClicked()
{
	OnChooseCampaignButton.Broadcast();
}

void ULobbyManagingWidgetBase::OnStartCampaignClicked()
{
	OnStartCampaignButton.Broadcast();
	BTN_StartCampaign->SetIsEnabled(false);
	BTN_StopCampaign->SetIsEnabled(true);
	BTN_ChangeCampaign->SetIsEnabled(false);

	SupervisedButtonState = BTN_Supervised->GetIsEnabled();
	UnsupervisedButtonState = BTN_Unsupervised->GetIsEnabled();

	BTN_Supervised->SetIsEnabled(false);
	BTN_Unsupervised->SetIsEnabled(false);

}

void ULobbyManagingWidgetBase::OnStopCampaignClicked()
{
	OnStopCampaignButton.Broadcast();
	BTN_StartCampaign->SetIsEnabled(true);
	BTN_StopCampaign->SetIsEnabled(false);
	BTN_ChangeCampaign->SetIsEnabled(true);

	BTN_Supervised->SetIsEnabled(SupervisedButtonState);
	BTN_Unsupervised->SetIsEnabled(UnsupervisedButtonState);
}

void ULobbyManagingWidgetBase::OnSupervisedClicked()
{
	OnSupervisedChanged.Broadcast(true);
	BTN_Supervised->SetIsEnabled(false);
	BTN_Unsupervised->SetIsEnabled(true);

}

void ULobbyManagingWidgetBase::OnUnsupervisedClicked()
{
	OnSupervisedChanged.Broadcast(false);
	BTN_Supervised->SetIsEnabled(true);
	BTN_Unsupervised->SetIsEnabled(false);
}

void ULobbyManagingWidgetBase::OnGameClicked(UUserWidget* Widget)
{
	OnActiveGameChanged.Broadcast(SCRB_Games->GetChildIndex(Widget));
}
