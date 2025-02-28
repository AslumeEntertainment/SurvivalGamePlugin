// Fill out your copyright notice in the Description page of Project Settings.


//===================================TO BE DELETED==========================


#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "SG_GameInstance.generated.h"

class USessionSubsystem;
class UGlobalWidgetSubsystem;

UCLASS()
class SURVIVALGAMEPLUGIN_API USG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

protected:

public:

	virtual void Init() override;

	void OnStart() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USessionSubsystem> SessionSubsystemClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGlobalWidgetSubsystem> GlobalWidgetSubsytemClass;

};