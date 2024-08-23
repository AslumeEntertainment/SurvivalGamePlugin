// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Headers/SessionState.h"

#include "SG_PlayerController.generated.h"

UENUM(BlueprintType)
enum class EInputMode : uint8
{
	GameOnly,
	UIOnly,
	GameAndUI,
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FConnectionCleanupSignature, APlayerController*, TriggeringPlayer);

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Developer")
	bool bIsCamersMovementConstrained = true;
	
public:

	UFUNCTION(Client, Reliable)
	void Client_SetInputMode(EInputMode InputMode);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_LeaveSession(bool bWasKicked=false);

	void OnNetCleanup(UNetConnection* Connection) override;

	void OnPossess(APawn* InPawn) override;

	FConnectionCleanupSignature OnConnectionCleanUp;

};
