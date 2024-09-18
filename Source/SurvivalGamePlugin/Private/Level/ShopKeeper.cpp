// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/ShopKeeper.h"

#include "GameFoundation/PlayerControllers/SG_PlayerController.h"
#include "GameFoundation/SaveGames/SG_SaveGame.h"
#include "UI/Widgets/ShopWidgetBase.h"
#include "UI/Widgets/ShopNotificationWidgetBase.h"
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
	NotificationWidget = CreateWidget<UShopNotificationWidgetBase>(World, ShopNotificationWidgetClass);

	if (IsValid(ShopWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BeginPlay IsValid(ShopWidget) == false"));
		return;
	}

	if (IsValid(NotificationWidget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AShopKeeper::BeginPlay IsValid(NotificationWidget) == false"));
		return;
	}

	ShopWidget->OnExitButton.AddDynamic(this, &AShopKeeper::StopInteracting);
	ShopWidget->OnPackSelected.AddDynamic(this, &AShopKeeper::BoosterPackSelected);
	ShopWidget->OnHeroCardSelected.AddDynamic(this, &AShopKeeper::HeroCardSelected);
	ShopWidget->OnResourceCardSelected.AddDynamic(this, &AShopKeeper::ResourceCardSelected);

	NotificationWidget->OnCancelButton.AddDynamic(this, &AShopKeeper::HideNotification);
}

void AShopKeeper::InteractRequest(AController* InteractingCharacter)
{
	InteractingPlayer = Cast<ASG_PlayerController>(InteractingCharacter);
	if (IsValid(InteractingPlayer) == false)
	{
		return;
	}
	InteractingPlayer->OnSaveUpdated.AddDynamic(this, &AShopKeeper::UpdateWidgets);
	InteractingPlayer->Client_SetInputMode(EInputMode::UIOnly);

	UpdateWidgets();

	ShopWidget->AddToViewport();
}

bool AShopKeeper::bCanInteract(AController* InteractingCharacter)
{
	return true;
}

void AShopKeeper::StopInteracting()
{
	InteractingPlayer->OnSaveUpdated.RemoveAll(this);
	InteractingPlayer->Client_SetInputMode(EInputMode::GameOnly);
	ShopWidget->RemoveFromParent();
	InteractingPlayer = nullptr;

	ActiveIndex = -1;

}

void AShopKeeper::BoosterPackSelected(int Index)
{
	ActiveIndex = Index;

	NotificationWidget->OnOkButton.AddDynamic(this, &AShopKeeper::BuyBoosterPack);

	bool CanAfford = BoosterCosts[Index] <= InteractingPlayer->GetSave()->Money;

	ShowNotification(CanAfford, BuyingAction, BoosterNames[Index], BoosterCosts[Index]);

}

void AShopKeeper::BuyBoosterPack()
{
	FBoosterPackPersistentData* Pack = BoosterPackTable->FindRow<FBoosterPackPersistentData>(BoosterIDs[ActiveIndex], "Buying Pack");

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
				FProgressiveDataHelperStruct Data;
				Data.ID = Entry.RepresentedUnit.RowName;
				Data.Level = 0;
				Save->HeroCards.Add(Data);
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
				Save->ResourceCards.Add(Entry.RepresentedUnit.RowName);
				break;
			}
		}
	}

	for (int i = 0; i < Pack->NumberOfCampaigns; i++)
	{
		RNG = FMath::RandRange(1, MaxRNGRange);
		for (uint16 j = 0; j < Pack->PossibleCampaigns.Num(); j++)
		{
			Entry = Pack->PossibleCampaigns[j];
			if (RNG <= Entry.Chance)
			{
				Drops.Add(Entry.RepresentedUnit);
				FProgressiveDataHelperStruct Data;
				Data.ID = Entry.RepresentedUnit.RowName;
				Data.Level = 0;
				Save->Campaigns.Add(Data);
				break;
			}
		}
	}

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

	HideNotification();

	StartLootingAnimation(Drops);
}

void AShopKeeper::HeroCardSelected(int Index)
{
	ActiveIndex = Index;

	NotificationWidget->OnOkButton.AddDynamic(this, &AShopKeeper::SellHeroCard);

	ShowNotification(true, SellingAction, HeroNames[Index], HeroCosts[Index]);
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

	Save->HeroCards.RemoveAt(ActiveIndex);

	InteractingPlayer->WriteSave();

	/*HeroIcons.RemoveAt(ActiveIndex);
	HeroNames.RemoveAt(ActiveIndex);
	HeroCosts.RemoveAt(ActiveIndex);
	ShopWidget->RemoveHeroCard(ActiveIndex);*/

	HideNotification();
}

void AShopKeeper::ResourceCardSelected(int Index)
{
	ActiveIndex = Index;

	NotificationWidget->OnOkButton.AddDynamic(this, &AShopKeeper::SellResourceCard);

	ShowNotification(true, SellingAction, ResourceNames[Index], ResourceCosts[Index]);
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

	Save->ResourceCards.RemoveAt(ActiveIndex);

	InteractingPlayer->WriteSave();

	/*ResourceIcons.RemoveAt(ActiveIndex);
	ResourceNames.RemoveAt(ActiveIndex);
	ResourceCosts.RemoveAt(ActiveIndex);
	ShopWidget->RemoveResourceCard(ActiveIndex);*/

	HideNotification();
}

void AShopKeeper::StartLootingAnimation(TArray<FDataTableRowHandle> Drops)
{
	TArray<FText> DropNames;
	TArray<UTexture2D*> DropImages;
	FPersistentDataUnit* TempUnit;
	for (FDataTableRowHandle Drop : Drops)
	{
		TempUnit = Drop.GetRow<FPersistentDataUnit>("Getting icon for drop");
		DropNames.Add(TempUnit->UnitName);
		DropImages.Add(TempUnit->UnitImage);
	}


	//start animation
}

void AShopKeeper::ShowNotification(bool CanAfford, FText Action, FText Object, uint16 Cost)
{
	NotificationWidget->Config(CanAfford, Action, Object, Cost);
	NotificationWidget->AddToViewport();
}

void AShopKeeper::HideNotification()
{
	NotificationWidget->RemoveFromParent();
	NotificationWidget->OnOkButton.RemoveAll(this);
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
	BoosterCosts.Empty();
	HeroNames.Empty();
	HeroIcons.Empty();
	HeroCosts.Empty();
	ResourceNames.Empty();
	ResourceIcons.Empty();
	ResourceCosts.Empty();

	TArray<FBoosterPackPersistentData*> Rows;
	BoosterPackTable->GetAllRows<FBoosterPackPersistentData>("GettingBoosterPacks", Rows);

	for (FBoosterPackPersistentData* Row : Rows)
	{
		BoosterIDs.Add(Row->ID);
		BoosterNames.Add(Row->UnitName);
		BoosterIcons.Add(Row->UnitImage);
		BoosterCosts.Add(Row->Cost);
	}

	USG_SaveGame* Save = InteractingPlayer->GetSave();

	for (FProgressiveDataHelperStruct Hero : Save->HeroCards)
	{
		FHeroCardPersistentData* HeroCard = HeroTable->FindRow<FHeroCardPersistentData>(Hero.ID, "Filling Heroes");

		uint16* HeroCostPtr = HeroCard->ValuePerLevel.Find(Hero.Level);
		if (HeroCostPtr == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Hero[%s] doesn't have value for level %d"), *Hero.ID.ToString(), Hero.Level);
			return;
		}

		HeroNames.Add(HeroCard->UnitName);
		HeroIcons.Add(HeroCard->UnitImage);
		HeroCosts.Add(*HeroCostPtr);
	}

	for (FName Resource : Save->ResourceCards)
	{
		FResourseCardPersistentData* ResourceCard = ResourceTable->FindRow<FResourseCardPersistentData>(Resource, "Filling Resources");

		ResourceNames.Add(ResourceCard->UnitName);
		ResourceIcons.Add(ResourceCard->UnitImage);
		ResourceCosts.Add(ResourceCard->Value);
	}

	ShopWidget->FillBoosters(BoosterNames, BoosterIcons, BoosterCosts);
	ShopWidget->FillSells(HeroNames, HeroIcons, HeroCosts, ResourceNames, ResourceIcons, ResourceCosts);
}