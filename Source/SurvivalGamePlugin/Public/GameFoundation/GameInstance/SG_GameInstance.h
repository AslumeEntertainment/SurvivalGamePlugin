// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "SG_GameInstance.generated.h"

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

protected:

public:

	void OnStart() override;

};