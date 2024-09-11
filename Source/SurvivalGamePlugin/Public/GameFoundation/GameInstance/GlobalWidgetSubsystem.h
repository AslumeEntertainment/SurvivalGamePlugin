// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalWidgetSubsystem.generated.h"

class UAnnouncementWidgetBase;
class IGameMoviePlayer;

USTRUCT()
struct FLoadingScreenData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> LoadingScreenClass;

	UPROPERTY(EditAnywhere)
	bool bAutoCompleteWhenLoadingCompletes = true;

	UPROPERTY(EditAnywhere)
	bool bWaitForManualStop = false;

	UPROPERTY(EditAnywhere)
	bool bAllowEngineTick = true;

	UPROPERTY(EditAnywhere)
	int MinimumLoadingScreenDisplayTime = 5;

	UPROPERTY(EditAnywhere)
	bool bMoviesAreSkippable = true;

	UPROPERTY(EditAnywhere)
	TArray<FString> MoviesToPlay;

};

UCLASS(Blueprintable)
class SURVIVALGAMEPLUGIN_API UGlobalWidgetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	IGameMoviePlayer* MoviePlayer = nullptr;
	UAnnouncementWidgetBase* AnnouncementWidget = nullptr;
	UUserWidget* LoadingScreenWidget = nullptr;

	bool bShouldShowAnnouncementOnload = false;

	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	bool ShouldCreateSubsystem(UObject* Outer) const override;

	void ShowAnnouncementWidget(FString AnnouncementString);

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnnouncementWidgetBase> AnnouncementWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FLoadingScreenData> LoadingScreenPacks;
	
	UFUNCTION()
	virtual void BeginLoadingScreen(FString LoadingScreenPackName);

	UFUNCTION()
	virtual void EndLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void RequestAnnouncement(FString Announcement, bool ShouldShowAsSoonAsPossible);

	UFUNCTION(BlueprintCallable)
	void HideAnnouncementWidget();
};
