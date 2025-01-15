// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Stations/Station.h"

#include "Headers/GeneralDelegates.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "Interfaces/InteractableInterface.h"

#include "LobbyManager.generated.h"

class AGameManager;
class UDataTable;
class UCampaignChooserWidgetBase;
class ULobbyManagingWidgetBase;
class UWarningWidgetBase;

struct PerCampaignData
{
	FCampaignPersistentData Campaign;

	uint8 Progression = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FCampaignRepresentingSignature, FText, Name, uint8, PlayerNum, bool, IsSupervised, bool, IsChangeable, TArray<FString>, GameNames, uint8, Progression, uint8, CurrentGame);

UCLASS()
class SURVIVALGAMEPLUGIN_API ALobbyManager : public AStation
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyManager();

private:
	UPROPERTY()
	UCampaignChooserWidgetBase* CampaignChooserWidget;
	UPROPERTY()
	ULobbyManagingWidgetBase* LobbyManagingWidget;
	UPROPERTY()
	UWarningWidgetBase* LeavingGameWarningWidget = nullptr;

	UPROPERTY()
	TArray<uint16> CampaignIDs;
	UPROPERTY()
	TArray<FText> CampaignNames;
	UPROPERTY()
	TArray<UTexture2D*> CampaignIcons;
	UPROPERTY()
	TArray<FLinearColor> CampaignColors;
	UPROPERTY()
	TArray<FString> CampaignCodes;
	UPROPERTY()
	TArray<uint8> CampaignPlayerNums;
	UPROPERTY()
	TArray<uint8> CampaignProgressions;

	uint16 CurrentCampaignID;
	FText CurrentCampaignName;
	uint8 NumPlayers = 0;
	bool bIsSupervised = false;
	bool CurrentSupervisionChangeable = false;
	uint8 CurrentGame = 0;
	uint8 CurrentMaxProgression = 0;
	TArray<FString> CurrentGameNames;
	TArray<FString> CurrentGames;

	UPROPERTY()
	AGameManager* ActiveGameManager = nullptr;

	UFUNCTION()
	void ShowLeavingGameWarning();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldPrintDebug = true;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* CampaignTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UCampaignChooserWidgetBase> CampaignChooserWidgetBaseClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<ULobbyManagingWidgetBase> LobbyManagingWidgetBaseClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UWarningWidgetBase> LeavingGameWarningWidgetClass;

	UPROPERTY(EditAnywhere, Category = "GameManaging")
	TMap<uint8, AGameManager*> TablesByPlayerNum;

	void SetupManagingWidget();
	void SetupSelectorWidget();

	UFUNCTION(BlueprintCallable)
	void OpenPlaySpace();

	UFUNCTION(BlueprintCallable)
	void ClosePlaySpace();

	UFUNCTION(BlueprintCallable)
	void LeaveLobby();

	UFUNCTION()
	void SelectCampaign();

	UFUNCTION()
	void CampaignSelected(int Index);

	UFUNCTION()
	void SetCurrentGame(int Index);

	UFUNCTION()
	void ProgressGame();

	FCampaignRepresentingSignature OnCampaignFillNeeded;

public:

	UFUNCTION()
	void SetSupervision(bool State);

	UFUNCTION()
	void SendCampaign();

	void StopInteracting() override;
	virtual bool InteractRequest(AController* InteractingCharacter) override;
	virtual FText GetInteractableName() override { return FText::FromString("Manage Campaigns"); }

};
