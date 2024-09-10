// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/GlobalWidgetSubsystem.h"

#include "Kismet/GameplayStatics.h" 

#include "UI/Widgets/CoverUpWidgetBase.h"

DEFINE_LOG_CATEGORY_STATIC(GlobalWidgetSubsystem, Log, All);

void UGlobalWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("Initialize IsValid(World) == false"));
		return;
	}

	CoverupWidget = CreateWidget<UCoverUpWidgetBase>(UGameplayStatics::GetGameInstance(World), CoverUpWidgetClass);

	if (IsValid(CoverupWidget) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("Initialize IsValid(CoverupWidget) == false"));
		return;
	}

	GameViewport = World->GetGameViewport();
	if (IsValid(GameViewport) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("Initialize IsValid(GameViewport) == false"));
		return;
	}

}

void UGlobalWidgetSubsystem::Deinitialize()
{
	CoverupWidget->Destruct();
	CoverupWidget = nullptr;
}

bool UGlobalWidgetSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	return false;
}

void UGlobalWidgetSubsystem::ShowAnnouncementWidget(FString AnnouncementString)
{
	if (IsValid(CoverupWidget) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("ShowAnnouncementWidget IsValid(CoverupWidget) == false"));
		return;
	}

	/*if (IsValid(GameViewport) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("ShowAnnouncementWidget IsValid(GameViewport) == false"));
		return;
	}*/

	CoverupWidget->SetAnnouncementTextFromString(AnnouncementString);
	if (!CoverupWidget->IsInViewport())
	{
		CoverupWidget->AddToViewport();
	}
}

void UGlobalWidgetSubsystem::HideAnnouncementWidget()
{
	if (IsValid(CoverupWidget) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("HideAnnouncementWidget IsValid(CoverupWidget) == false"));
		return;
	}

	/*if (IsValid(GameViewport) == false)
	{
		UE_LOG(GlobalWidgetSubsystem, Error, TEXT("HideAnnouncementWidget IsValid(GameViewport) == false"));
		return;
	}*/

	CoverupWidget->SetAnnouncementTextFromString("No Announcement");
	CoverupWidget->RemoveFromViewport();
}
