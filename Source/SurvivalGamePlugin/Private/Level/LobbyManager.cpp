// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LobbyManager.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

#include "GameFoundation/GameInstance/SessionSubsystem.h"
#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/GameInstance/SG_GameInstance.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "Level/GameManager.h"
#include "Headers/GameManagingStructs.h"
#include "UI/Widgets/LobbyManagingWidgetBase.h"
#include "UI/Widgets/CampaignChooserWidgetBase.h"

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALobbyManager::BeginPlay()
{
	Super::BeginPlay();

	SetupManagingWidget();
}

void ALobbyManager::OpenPlaySpace()
{
	if (HasAuthority() == false)
	{
		return;
	}

	USG_GameInstance* GameInstance = Cast<USG_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::CreateLobby IsValid(GameInstance) == false - GameInstance may not be of class USG_GameInstance"));
		return;
	}

	uint8 NumConnections;
	switch (NumPlayers)
	{
		case 1: case 2: NumConnections = 2; break;
		case 3: case 4: NumConnections = 4; break;
		case 5: case 6: NumConnections = 6; break;
		case 7: case 8: NumConnections = 8; break;
		default: NumConnections = 0;
	}

	AGameManager** GameManagerPtr = TablesByPlayerNum.Find(NumConnections);
	if (GameManagerPtr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Table for %d players"), NumConnections);
		return;
	}
	ActiveGameManager = *GameManagerPtr;
	ActiveGameManager->Open(bIsSupervised, CurrentGames[CurrentGame]);

	StopInteracting();
}

void ALobbyManager::ClosePlaySpace()
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (IsValid(ActiveGameManager) == false)
	{
		return;
	}
	ActiveGameManager->Close();
	ActiveGameManager = nullptr;
}

void ALobbyManager::LeaveLobby()
{
	ASG_PlayerController* PC = Cast<ASG_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	PC->Client_LeaveSession(true);
	if (IsValid(ActiveGameManager) == false)
	{
		return;
	}
	ActiveGameManager->Close();
	ActiveGameManager = nullptr;
}

void ALobbyManager::SelectCampaign()
{
	CampaignChooserWidget->Show();
}

void ALobbyManager::CampaignSelected(int Index)
{


	FString Code = CampaignCodes[Index];

	CurrentGames.Empty();

	FText ExtractedCampaignName;
	FString TempString = "";
	uint8 ExtractedNumPlayers = 0;
	bool ExtractedIsSupervised = false;
	bool SupervisionChangeable = false;

	int DelimiterIndex = 0;

	//Getting the name of the Campaign
	Code.FindChar(':', DelimiterIndex);
	FString TempStr = Code.Left(DelimiterIndex);
	ExtractedCampaignName = (TempStr == "" ? CampaignNames[Index] : FText::FromString(TempStr));
	Code = Code.Mid(DelimiterIndex + 1);

	//Getting the number of players
	Code.FindChar(':', DelimiterIndex);
	TempStr = Code.Left(DelimiterIndex);
	ExtractedNumPlayers = (TempStr == "" ? CampaignPlayerNums[Index] : UKismetStringLibrary::Conv_StringToInt(TempStr));
	Code = Code.Mid(DelimiterIndex + 1);

	//getting the supervision status
	Code.FindChar(':', DelimiterIndex);
	TempString = Code.Left(DelimiterIndex);
	if (TempString == "c")
	{
		SupervisionChangeable = true;
	}
	ExtractedIsSupervised = TempString.ToBool();
	Code = Code.Mid(DelimiterIndex + 1);

	bIsSupervised = ExtractedIsSupervised;
	NumPlayers = ExtractedNumPlayers;
	CurrentGame = CampaignProgressions[Index];
	OnCampaignFillNeeded.Broadcast(ExtractedCampaignName, ExtractedNumPlayers, ExtractedIsSupervised, SupervisionChangeable);

	uint8 ExtractedNumberOfGames = 0;

	//Getting the number of games
	Code.FindChar('[', DelimiterIndex);
	ExtractedNumberOfGames = UKismetStringLibrary::Conv_StringToInt(Code.Left(DelimiterIndex));
	Code = Code.Mid(DelimiterIndex + 1);

	UE_LOG(LogTemp, Error, TEXT("Code: %s"), *Code);

	FString GameString;

	for (uint8 i = 0; i < ExtractedNumberOfGames; i++)
	{
		Code.FindChar('-', DelimiterIndex);
		GameString = Code.Left(DelimiterIndex);
		Code = Code.Mid(DelimiterIndex + 1);

		UE_LOG(LogTemp, Error, TEXT("Game %d: %s"), i, *GameString);
		CurrentGames.Add(GameString);
	}

	CampaignChooserWidget->Hide();
}

void ALobbyManager::SetSupervision(bool State)
{
	bIsSupervised = State;
	LobbyManagingWidget->SetSupervised(bIsSupervised);
}

void ALobbyManager::InteractRequest(AController* InteractingCharacter)
{
	InteractingPlayer = Cast<ASG_PlayerController>(InteractingCharacter);
	if (IsValid(InteractingPlayer) == false)
	{
		return;
	}
	InteractingPlayer->Client_SetInputMode(EInputMode::UIOnly);
	
	SetupSelectorWidget();

	LobbyManagingWidget->Show();
}

bool ALobbyManager::bCanInteract(AController* InteractingCharacter)
{
	return true;
}

void ALobbyManager::StopInteracting()
{
	InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	InteractingPlayer = nullptr;

	CampaignChooserWidget->MarkForDestruction();
	CampaignChooserWidget = nullptr;

	LobbyManagingWidget->Hide();

	CampaignNames.Empty();
	CampaignIcons.Empty();
	CampaignCodes.Empty();

}

void ALobbyManager::SetupManagingWidget()
{

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::SetupWidgets IsValid(World) == false"));
		return;
	}

	LobbyManagingWidget = CreateWidget<ULobbyManagingWidgetBase>(World, LobbyManagingWidgetBaseClass);

	if (IsValid(LobbyManagingWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::SetupWidgets IsValid(LobbyManagingWidget) == false"));
		return;
	}

	OnCampaignFillNeeded.AddDynamic(LobbyManagingWidget, &ULobbyManagingWidgetBase::FillCampaign);

	LobbyManagingWidget->OnChooseCampaignButton.AddDynamic(this, &ALobbyManager::SelectCampaign);
	LobbyManagingWidget->OnStartCampaignButton.AddDynamic(this, &ALobbyManager::OpenPlaySpace);
	LobbyManagingWidget->OnStopCampaignButton.AddDynamic(this, &ALobbyManager::ClosePlaySpace);
	LobbyManagingWidget->OnExitButton.AddDynamic(this, &ALobbyManager::StopInteracting);
	LobbyManagingWidget->OnLeaveButton.AddDynamic(this, &ALobbyManager::LeaveLobby);
	LobbyManagingWidget->OnSupervisedChanged.AddDynamic(this, &ALobbyManager::SetSupervision);

}

void ALobbyManager::SetupSelectorWidget()
{
	USG_SaveGame* Save = InteractingPlayer->GetSave();

	for (FProgressiveDataHelperStruct Campaign : Save->Campaigns)
	{
		FCampaignPersistentData* CampaignCard = CampaignTable->FindRow<FCampaignPersistentData>(Campaign.ID, "Filling Campaign");

		CampaignNames.Add(CampaignCard->UnitName);
		CampaignIcons.Add(CampaignCard->UnitImage);
		CampaignCodes.Add(CampaignCard->Code);
		CampaignPlayerNums.Add(CampaignCard->DevPlayerNum);
		CampaignProgressions.Add(Campaign.Level);
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::SetupSelectorWidget IsValid(World) == false"));
		return;
	}

	CampaignChooserWidget = CreateWidget<UCampaignChooserWidgetBase>(World, CampaignChooserWidgetBaseClass);

	if (IsValid(CampaignChooserWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ALobbyManager::SetupSelectorWidget IsValid(CampaignSelectorWidget) == false"));
		return;
	}

	CampaignChooserWidget->FillCampaigns(CampaignNames, CampaignIcons, CampaignPlayerNums);

	CampaignChooserWidget->OnCampaignSelected.AddDynamic(this, &ALobbyManager::CampaignSelected);
	CampaignChooserWidget->OnExitButton.AddDynamic(CampaignChooserWidget, &UCampaignChooserWidgetBase::RemoveFromParent);
}


