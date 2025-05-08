// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/Stations/ShopKeeper.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "UI/Widgets/Core/WarningWidgetBase.h"
#include "UI/Widgets/Lobby/Shop/ShopWidgetBase.h"
#include "Headers/GameManagingPersistentStructs.h"

// Sets default values
AShopKeeper::AShopKeeper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AShopKeeper::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BeginPlay IsValid(World) == false"));
		return;
	}

	ShopWidget = CreateWidget<UShopWidgetBase>(World, ShopWidgetClass);
	ShopWarningWidget = CreateWidget<UWarningWidgetBase>(World, ShopWarningWidgetClass);

	if (IsValid(ShopWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BeginPlay IsValid(ShopWidget) == false"));
		return;
	}

	if (IsValid(ShopWarningWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BeginPlay IsValid(NotificationWidget) == false"));
		return;
	}

	ShopWidget->OnExitButton.AddDynamic(this, &AShopKeeper::StopInteracting);
	ShopWidget->OnPackSelected.AddDynamic(this, &AShopKeeper::BoosterPackSelected);
	ShopWidget->OnHeroCardSelected.AddDynamic(this, &AShopKeeper::HeroCardSelected);
	ShopWidget->OnResourceCardSelected.AddDynamic(this, &AShopKeeper::ResourceCardSelected);

	ShopWarningWidget->OnCancelButtonClicked.AddDynamic(this, &AShopKeeper::HideNotification);
}

bool AShopKeeper::InteractRequest(AController* InteractingCharacter)
{
	if (InteractingCharacter->IsLocalPlayerController() == false) return false;
	
	if (Super::InteractRequest(InteractingCharacter) == false) return false;

	InteractingPlayer->OnSaveUpdated.AddDynamic(this, &AShopKeeper::UpdateWidgets);

	UpdateWidgets();

	ShopWidget->AddToViewport();

	return true;
}

void AShopKeeper::StopInteracting()
{

	InteractingPlayer->OnSaveUpdated.RemoveAll(this);

	Super::StopInteracting();

	ShopWidget->RemoveFromParent();

	ActiveIndex = -1;

	BoosterIDs.Empty();
	BoosterNames.Empty();
	BoosterIcons.Empty();
	BoosterColors.Empty();
	BoosterCosts.Empty();
	HeroNames.Empty();
	HeroIcons.Empty();
	HeroColors.Empty();
	HeroCosts.Empty();
	ResourceNames.Empty();
	ResourceIcons.Empty();
	ResourceColors.Empty();
	ResourceCosts.Empty();
}

void AShopKeeper::BoosterPackSelected(int Index)
{
	ActiveIndex = Index;

	ShopWarningWidget->OnConfirmButtonClicked.AddDynamic(this, &AShopKeeper::BuyBoosterPack);

	bool CanAfford = BoosterCosts[Index] <= InteractingPlayer->GetSave()->Money;
	FText Message = FText::FromString(WarningMessageBeginning + " " + BuyingAction + " " + BoosterNames[Index].ToString() 
		+ " for " + FString::FromInt(BoosterCosts[Index]) + "?");

	ShowNotification(Message, CanAfford);

}

void AShopKeeper::BuyBoosterPack()
{
	FBoosterPackPersistentData* Pack = nullptr;

	TArray<FBoosterPackPersistentData*> PackDataRows;
	BoosterPackTable->GetAllRows<FBoosterPackPersistentData>("Buying Pack", PackDataRows);

	for (FBoosterPackPersistentData* PackRow : PackDataRows)
	{
		if (PackRow->ID != BoosterIDs[ActiveIndex]) continue;
		Pack = PackRow;
	}

	if (Pack == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BuyBoosterPack (Pack == nullptr Could not find pack with id - %d which is at index - %d"), BoosterIDs[ActiveIndex], ActiveIndex);
		return;
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();

	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BuyBoosterPack IsValid(Save) == false"));
		return;
	}

	Save->Money -= Pack->Cost;
	
	int RNG = 0;

	TArray<FDataTableRowHandle> Drops;
	FBoosterPackHelperStruct Entry;

	for (int i = 0; i < Pack->NumberOfHeroes; i++)
	{
		RNG = FMath::RandRange(1, MaxRNGRange);
		for (uint16 j = 0; j<Pack->PossibleHeroes.Num(); j++)
		{
			Entry = Pack->PossibleHeroes[j];
			if (RNG <= Entry.Chance)
			{
				Drops.Add(Entry.RepresentedUnit);

				FHeroCardPersistentData* Data = Entry.RepresentedUnit.GetRow<FHeroCardPersistentData>("Winning Hero");
				FProgressiveDataHelperStruct* FoundEmptyHero = Save->HeroCards.FindByPredicate([](FProgressiveDataHelperStruct& EmptyHero) { return EmptyHero.ID == 0; });
				if (FoundEmptyHero != nullptr)
				{
					FoundEmptyHero->ID = Data->ID;
				}
				break;
			}
		}
	}

	for (int i = 0; i < Pack->NumberOfResources; i++)
	{
		RNG = FMath::RandRange(1, MaxRNGRange);
		for (uint16 j = 0; j < Pack->PossibleResources.Num(); j++)
		{
			Entry = Pack->PossibleResources[j];
			if (RNG <= Entry.Chance)
			{
				Drops.Add(Entry.RepresentedUnit);
				FResourseCardPersistentData* Data = Entry.RepresentedUnit.GetRow<FResourseCardPersistentData>("Winning Resource");
				FProgressiveDataHelperStruct* FoundEmptyResource = Save->ResourceCards.FindByPredicate( [](FProgressiveDataHelperStruct& EmptyResource) { return EmptyResource.ID == 0; } );
				if (FoundEmptyResource != nullptr)
				{
					FoundEmptyResource->ID = Data->ID;
				}
				break;
			}
		}
	}

	//for (int i = 0; i < Pack->NumberOfCampaigns; i++)
	//{
		RNG = FMath::RandRange(1, MaxRNGRange);
		for (uint16 j = 0; j < Pack->PossibleCampaigns.Num(); j++)
		{
			Entry = Pack->PossibleCampaigns[j];
			if (RNG <= Entry.Chance)
			{
				Drops.Add(Entry.RepresentedUnit);
				FProgressiveDataHelperStruct Data;
				Data.ID = Entry.RepresentedUnit.GetRow<FCampaignPersistentData>("Campaign Hero")->ID;
				Data.Level = 0;
				Save->Campaigns.Add(Data);
				//break;
			}
		}
	//}

	for (int i = 0; i < Pack->NumberOfBiomes; i++)
	{
		RNG = FMath::RandRange(1, MaxRNGRange);
		for (uint16 j=0; j < Pack->PossibleBiomes.Num(); j++ )
		{
			Entry = Pack->PossibleBiomes[j];
			if (RNG <= Entry.Chance)
			{
				Drops.Add(Entry.RepresentedUnit);
				Save->Biomes.Add(Entry.RepresentedUnit.RowName);
				break;
			}
		}
	}

	InteractingPlayer->WriteSave();
	UpdateWidgets();
	HideNotification();

	StartLootingAnimation(Drops);
}

void AShopKeeper::HeroCardSelected(int Index)
{
	ActiveIndex = Index;

	ShopWarningWidget->OnConfirmButtonClicked.AddDynamic(this, &AShopKeeper::SellHeroCard);

	FText Message = FText::FromString(WarningMessageBeginning + " " + SellingAction + " " + HeroNames[Index].ToString()
		+ " for " + FString::FromInt(HeroCosts[Index]) + "?");

	ShowNotification(Message, true);
}

void AShopKeeper::SellHeroCard()
{
	USG_SaveGame* Save = InteractingPlayer->GetSave();

	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BuyBoosterPack IsValid(Save) == false"));
		return;
	}

	Save->Money += HeroCosts[ActiveIndex];

	Save->HeroCards[HeroIndexes[ActiveIndex]] = FProgressiveDataHelperStruct();

	InteractingPlayer->WriteSave();
	UpdateWidgets();
	HideNotification();
}

void AShopKeeper::ResourceCardSelected(int Index)
{
	ActiveIndex = Index;

	ShopWarningWidget->OnConfirmButtonClicked.AddDynamic(this, &AShopKeeper::SellResourceCard);

	FText Message = FText::FromString(WarningMessageBeginning + " " + SellingAction + " " + ResourceNames[Index].ToString()
		+ " for " + FString::FromInt(ResourceCosts[Index]) + "?");

	ShowNotification(Message, true);
}

void AShopKeeper::SellResourceCard()
{
	USG_SaveGame* Save = InteractingPlayer->GetSave();

	if (IsValid(Save) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BuyBoosterPack IsValid(Save) == false"));
		return;
	}

	Save->Money += ResourceCosts[ActiveIndex];

	Save->ResourceCards[ResourceIndexes[ActiveIndex]] = FProgressiveDataHelperStruct();

	InteractingPlayer->WriteSave();
	UpdateWidgets();
	HideNotification();
}

void AShopKeeper::StartLootingAnimation(TArray<FDataTableRowHandle> Drops)
{
	TArray<FText> DropNames;
	TArray<UTexture2D*> DropImages;
	FPersistentDataUnit* TempUnit;
	for (const FDataTableRowHandle Drop : Drops)
	{
		TempUnit = Drop.GetRow<FPersistentDataUnit>("Getting icon for drop");
		DropNames.Add(TempUnit->UnitName);
		DropImages.Add(TempUnit->UnitImage);
	}

	//start animation
}

void AShopKeeper::ShowNotification(FText Message, bool CanAfford)
{
	ShopWarningWidget->ConfigWarning(Message, CanAfford);
	ShopWarningWidget->AddToViewport();
}

void AShopKeeper::HideNotification()
{
	ShopWarningWidget->RemoveFromParent();
	ShopWarningWidget->OnConfirmButtonClicked.RemoveAll(this);
	ActiveIndex = -1;
}

void AShopKeeper::UpdateWidgets()
{
	USG_SaveGame* Save = InteractingPlayer->GetSave();
	ShopWidget->SetMoney(Save->Money);
	FillShopWidget();
}

void AShopKeeper::FillShopWidget()
{
	ActiveIndex = -1;

	BoosterIDs.Empty();
	BoosterNames.Empty();
	BoosterIcons.Empty();
	BoosterColors.Empty();
	BoosterCosts.Empty();
	HeroNames.Empty();
	HeroIcons.Empty();
	HeroColors.Empty();
	HeroCosts.Empty();
	HeroIndexes.Empty();
	ResourceNames.Empty();
	ResourceIcons.Empty();
	ResourceColors.Empty();
	ResourceCosts.Empty();
	ResourceIndexes.Empty();

	TArray<FBoosterPackPersistentData*> Rows;
	BoosterPackTable->GetAllRows<FBoosterPackPersistentData>("GettingBoosterPacks", Rows);

	for (FBoosterPackPersistentData* Row : Rows)
	{
		BoosterIDs.Add(Row->ID);
		BoosterNames.Add(Row->UnitName);
		BoosterIcons.Add(Row->UnitImage);
		BoosterColors.Add(Row->UnitColor);
		BoosterCosts.Add(Row->Cost);
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();

	TArray<FHeroCardPersistentData*> HeroDataRows;

	HeroTable->GetAllRows<FHeroCardPersistentData>("Filling Heroes", HeroDataRows);


	uint8 HeroIndex = -1;
	for (FProgressiveDataHelperStruct SavedHero : Save->HeroCards)
	{
		HeroIndex++;
		if (SavedHero.ID == 0) continue;
		
		FHeroCardPersistentData* FoundHeroDataPtr = HeroTable->FindRow<FHeroCardPersistentData>(FName(*FString::FromInt(SavedHero.ID)), "Filling Heroes");

		if (FoundHeroDataPtr == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Hero[%d] doesn't exist"), SavedHero.ID);
			continue;
		}

		uint16* HeroCostPtr = FoundHeroDataPtr->ValuePerLevel.Find(SavedHero.Level);
		if (HeroCostPtr == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Hero[%d] doesn't have value for level %d"), SavedHero.ID, SavedHero.Level);
			return;
		}

		HeroNames.Add(FoundHeroDataPtr->UnitName);
		HeroIcons.Add(FoundHeroDataPtr->UnitImage);
		HeroColors.Add(FoundHeroDataPtr->UnitColor);
		HeroCosts.Add(*HeroCostPtr);
		HeroIndexes.Add(HeroIndex);
	}

	uint16 ResourceIndex = -1;
	for (FProgressiveDataHelperStruct SavedResource : Save->ResourceCards)
	{
		ResourceIndex++;
		if (SavedResource.ID == 0) continue;

		FResourseCardPersistentData* FoundResourceDataPtr = ResourceTable->FindRow<FResourseCardPersistentData>(FName(*FString::FromInt(SavedResource.ID)), "Filling Resources");

		if (FoundResourceDataPtr == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Resource[%d] doesn't exist"), SavedResource.ID);
			continue;
		}

		ResourceNames.Add(FoundResourceDataPtr->UnitName);
		ResourceIcons.Add(FoundResourceDataPtr->UnitImage);
		ResourceColors.Add(FoundResourceDataPtr->UnitColor);
		ResourceCosts.Add(FoundResourceDataPtr->Value);
		ResourceIndexes.Add(ResourceIndex);
	}

	ShopWidget->FillBoosters(BoosterNames, BoosterIcons, BoosterColors, BoosterCosts);
	ShopWidget->FillHeroes(HeroNames, HeroIcons, HeroColors, HeroCosts);
	ShopWidget->FillResources(ResourceNames, ResourceIcons, ResourceColors, ResourceCosts);
}