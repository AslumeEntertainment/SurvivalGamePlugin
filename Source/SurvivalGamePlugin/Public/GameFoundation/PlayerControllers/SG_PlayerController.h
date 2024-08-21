// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SG_PlayerController.generated.h"

UENUM(BlueprintType)
enum class EInputMode : uint8
{
	GameOnly,
	UIOnly,
	GameAndUI,
};

UCLASS()
class SURVIVALGAMEPLUGIN_API ASG_PlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:

	UFUNCTION(Client, Reliable)
	void Client_SetInputMode(EInputMode InputMode);

	void OnNetCleanup(UNetConnection* Connection) override;

};
