// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalWidgetSubsystem.generated.h"

class UCoverUpWidgetBase;

UCLASS(Blueprintable)
class SURVIVALGAMEPLUGIN_API UGlobalWidgetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	UCoverUpWidgetBase* CoverupWidget = nullptr;
	UGameViewportClient* GameViewport = nullptr;

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	bool ShouldCreateSubsystem(UObject* Outer) const override;

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCoverUpWidgetBase> CoverUpWidgetClass;

	UFUNCTION(BlueprintCallable)
	void ShowAnnouncementWidget(FString AnnouncementString);

	UFUNCTION(BlueprintCallable)
	void HideAnnouncementWidget();
};
