// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/GlobalWidgetSubsystem.h"

#include "Kismet/GameplayStatics.h" 
#include "MoviePlayer.h"

#include "UI/Widgets/AnnouncementWidgetBase.h"

DEFINE_LOG_CATEGORY_STATIC(GlobalWidgetSubsystem, Log, All);

void UGlobalWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	Super::Initialize(Collection);

	MoviePlayer = GetMoviePlayer();
	if (MoviePlayer == nullptr)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("BeginLoadingScreen IsValid(MoviePlayer) == false"));
		return;
	}

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UGlobalWidgetSubsystem::BeginLoadingScreen);
}

void UGlobalWidgetSubsystem::Deinitialize()
{
	if (IsValid(LoadingScreenWidget) == true)
	{
		LoadingScreenWidget->Destruct();
	}
	LoadingScreenWidget = nullptr;

	if (IsValid(AnnouncementWidget) == true)
	{
		AnnouncementWidget->Destruct();
	}
	AnnouncementWidget = nullptr;

	MoviePlayer = nullptr;
}

bool UGlobalWidgetSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	return false;
}

void UGlobalWidgetSubsystem::BeginLoadingScreen(const FString& LoadingScreenPackName)
{


	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("BeginLoadingScreen IsValid(World) == false"));
		return;
	}

	FLoadingScreenData* LoadingScreenData = LoadingScreenPacks.Find(LoadingScreenPackName);
	if (LoadingScreenData == nullptr)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("BeginLoadingScreen No loading screen pack with name: %s"), *LoadingScreenPackName);
		return;
	}

	UE_LOG(GlobalWidgetSubsystem, Error, TEXT("BeginLoadingScreen SHOWING : %s"), *LoadingScreenPackName);

	LoadingScreenWidget = CreateWidget<UUserWidget>(World, LoadingScreenData->LoadingScreenClass);

	if (IsValid(LoadingScreenWidget) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("BeginLoadingScreen IsValid(LoadingScreenWidget) == false"));
		return;
	}

	if (MoviePlayer->IsMovieCurrentlyPlaying() == true)
	{
		MoviePlayer->StopMovie();
	}
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = LoadingScreenData->bAutoCompleteWhenLoadingCompletes;
	LoadingScreen.bWaitForManualStop = LoadingScreenData->bWaitForManualStop;
	LoadingScreen.bAllowEngineTick = LoadingScreenData->bAllowEngineTick;
	LoadingScreen.MinimumLoadingScreenDisplayTime = LoadingScreenData->MinimumLoadingScreenDisplayTime;
	LoadingScreen.bMoviesAreSkippable = LoadingScreenData->bMoviesAreSkippable;
	LoadingScreen.WidgetLoadingScreen = LoadingScreenWidget->TakeWidget();

	for (FString Movie : LoadingScreenData->MoviesToPlay)
	{
		LoadingScreen.MoviePaths.Add(Movie);
	}

	MoviePlayer->SetupLoadingScreen(LoadingScreen);
	//MoviePlayer->PlayMovie();
}

void UGlobalWidgetSubsystem::EndLoadingScreen()
{
	UE_LOG(GlobalWidgetSubsystem, Error, TEXT("BeginLoadingScreen STOPPIN CURRENT MOVIE"));
	MoviePlayer->StopMovie();
	MoviePlayer->ForceCompletion();

	if (IsValid(LoadingScreenWidget) == true)
	{
		LoadingScreenWidget->Destruct();
	}
	LoadingScreenWidget = nullptr;
}

void UGlobalWidgetSubsystem::RequestAnnouncement(FString Announcement, bool ShouldShowAsSoonAsPossible)
{
	//bShouldShowAnnouncementOnload.
}

void UGlobalWidgetSubsystem::ShowAnnouncementWidget(FString AnnouncementString)
{

}

void UGlobalWidgetSubsystem::HideAnnouncementWidget()
{

}
