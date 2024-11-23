// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Headers/GeneralDelegates.h"
#include "Headers/GameManagingPersistentStructs.h"
#include "Interfaces/InteractableInterface.h"

#include "LobbyManager.generated.h"

class AGameManager;
class UDataTable;
class ASG_PlayerController;
class UCampaignChooserWidgetBase;
class ULobbyManagingWidgetBase;

struct PerCampaignData
{
	FCampaignPersistentData Campaign;

	uint8 Progression = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FCampaignRepresentingSignature, FText, Name, uint8, PlayerNum, bool, IsSupervised, bool, IsChangeable, TArray<FString>, GameNames, uint8, Progression, uint8, CurrentGame);

UCLASS()
class SURVIVALGAMEPLUGIN_API ALobbyManager : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyManager();

private:

	UPROPERTY()
	ASG_PlayerController* InteractingPlayer = nullptr;
	UPROPERTY()
	UCampaignChooserWidgetBase* CampaignChooserWidget;
	UPROPERTY()
	ULobbyManagingWidgetBase* LobbyManagingWidget;

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

	UPROPERTY(EditAnywhere, Category = "GameManaging")
	TMap<uint8, AGameManager*> TablesByPlayerNum;

	void SetupManagingWidget();
	void SetupSelectorWidget();

	UFUNCTION()
	void StopInteracting();

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

	virtual void InteractRequest(AController* InteractingCharacter) override;
	virtual bool bCanInteract(AController* InteractingCharacter) override;
	virtual FText GetInteractableName() override { return FText::FromString("Manage Campaigns"); }

};
