// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Interfaces/AbilitySystemControllerInterface.h"
#include "Headers/GeneralDelegates.h"

#include "SG_PlayerController.generated.h"

class USG_AbilitySystemComponent;
class USG_SaveGame;

UENUM(BlueprintType)
enum class EInputMode : uint8
{
	GameOnly,
	UIOnly,
	GameAndUI,
};

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_PlayerController : public APlayerController, public IAbilitySystemControllerInterface
{
	GENERATED_BODY()
	
private:

	UPROPERTY(Replicated)
	USG_AbilitySystemComponent* ASC = nullptr;

protected:

	UPROPERTY()
	USG_SaveGame* Save = nullptr;

	UPROPERTY(EditAnywhere, Category = "Developer")
	bool bIsCameraMovementConstrained = true;
	
	UFUNCTION()
	void ReceiveReadSave(USaveGame* SaveGame);

public:

	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_SetInputMode(EInputMode InputMode);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_LeaveSession(bool bWasKicked=false);

	void OnNetCleanup(UNetConnection* Connection) override;

	void OnPossess(APawn* InPawn) override;

	virtual void InitASC(AActor* AvatarActor) override;

	virtual FActiveGameplayEffectHandle AddEffect(TSubclassOf<UGameplayEffect> GameplayEffect, uint8 Level = 1) override;

	virtual void RemoveEffect(FActiveGameplayEffectHandle RemovedEffectSpecHandle, int8 StacksToRemove = -1) override;

	virtual FGameplayAbilitySpecHandle AddAbility(TSubclassOf<USG_GameplayAbility> AbilityClass, uint8 Level = 1) override;

	virtual void RemoveAbility(FGameplayAbilitySpecHandle RemovedAbilitySpecHandle) override;

	//Work with save game
	UFUNCTION()
	void ReadSave();

	UFUNCTION()
	void WriteSave();

	USG_SaveGame* GetSave();

	FPlayerControllerTransferSignature OnConnectionCleanUp;

	FVoidDataTransferSignature OnReadSaveRequested;
	FSaveGameTransferSignature OnWriteSaveRequested;

	FVoidDataTransferSignature OnSaveUpdated;


	//----------------Execs--------------------------

	UFUNCTION(Exec)
	void Cheat_SetMoney(uint32 Money);

	UFUNCTION(Exec)
	void Cheat_ClearAccount();
};
