// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/InGameCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

#include "Interactables/Items/DamageDealingItem.h"
#include "Components/PassiveDamageDealingComponent.h"
#include "Components/DamageDealingComponent.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"


AInGameCharacter::AInGameCharacter()
{
	MainEquipmentSlotTag =                 FGameplayTag::RequestGameplayTag("EquipmentSlot.Main");
	SecondaryEquipmentSlotTag =            FGameplayTag::RequestGameplayTag("EquipmentSlot.Secondary");
	MixedEquipmentSlotTag =                FGameplayTag::RequestGameplayTag("EquipmentSlot.Mixed");
	CombinedEquipmentSlotTag =             FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined");
	CombinedMainEquipmentSlotTag =         FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined.Main");
	CombinedSecondaryEquipmentSlotTag =    FGameplayTag::RequestGameplayTag("EquipmentSlot.Combined.Secondary");

	LeftHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("LeftHandDamageOrigin"));
	LeftHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_l"));

	RightHandDamageDealingComponent = CreateDefaultSubobject<UDamageDealingComponent>(TEXT("RightHandDamageOrigin"));
	RightHandDamageDealingComponent->SetupAttachment(GetMesh(), TEXT("hand_r"));

	PassiveDamageDealingComponent = CreateDefaultSubobject<UPassiveDamageDealingComponent>(TEXT("PassiveDamageDealingComponent"));
	PassiveDamageDealingComponent->SetupAttachment(GetRootComponent());

	PassiveDamageDealingComponent->OnComponentBeginOverlap.AddDynamic(this, &AInGameCharacter::OnImpactReceived);
}

void AInGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AInGameCharacter, AttackSpeed, COND_None, REPNOTIFY_Always);
}

void AInGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() == false)
	{
		return;
	}
	if (HasAuthority())
	{
		BroadcastInitialValues();
	}
	else
	{
		FTimerHandle BroadcastInitialValuesTimerHandle;
		GetWorldTimerManager().SetTimer(BroadcastInitialValuesTimerHandle, this, &AInGameCharacter::Client_BroadcastInitialValues, 1, false);
	}
}

void AInGameCharacter::Client_OnUpdatePlayerHealth_Implementation(float CurrentHealth, float MaxHealth)
{
	OnHealthPercentUpdated.Broadcast(CurrentHealth / MaxHealth);
}

void AInGameCharacter::Client_OnUpdatePlayerHunger_Implementation(float CurrentHunger, float MaxHunger)
{
	OnHungerPercentUpdated.Broadcast(CurrentHunger / MaxHunger);
}

void AInGameCharacter::Client_OnUpdatePlayerEnergy_Implementation(float CurrentEnergy, float MaxEnergy)
{
	OnEnergyPercentUpdated.Broadcast(CurrentEnergy / MaxEnergy);
}

void AInGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::MainHandAction);
		EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::SecondaryHandAction);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

void AInGameCharacter::GrantTemporalInvinsibility(float Duration)
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::GrantTemporalInvinsibility IsValid(AbilitySystemComponent) == false"));
		return;
	}
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility")))
	{
		return;
	}

	AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility"));
	AbilitySystemComponent->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility"));
	
	FTimerHandle InvincibilityTimerHandle;
	GetWorldTimerManager().SetTimer(InvincibilityTimerHandle, this, &AInGameCharacter::RemoveInvincibility, Duration, false, 0.1f);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, TEXT("Player is now [TITLE CARD]"));
}

void AInGameCharacter::RemoveInvincibility()
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::RemoveInvincibility IsValid(AbilitySystemComponent) == false"));
		return;
	}

	AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility"));
	AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasInvincibility"));
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, TEXT("Player is no longer [TITLE CARD]"));
}

void AInGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (HasAuthority() == false)
	{
		return;
	}
	if (USG_AbilitySystemBlueprintLibrary::CalculateAndApplyFallDamage(this, AbilitySystemComponent))
	{
		GrantTemporalInvinsibility(5.f);
	}
}

void AInGameCharacter::OnImpactReceived(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (SweepResult.Component != nullptr && SweepResult.Component->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn) == ECollisionResponse::ECR_Ignore)
	{
		return;
	}

	if (USG_AbilitySystemBlueprintLibrary::CalculateAndApplyImpactDamage(this, AbilitySystemComponent))
	{
		GrantTemporalInvinsibility(5.f);
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("Impact recieved! Player velocity - %f ; OtherActor velocity - %f"), GetVelocity().Length(), OtherActor->GetVelocity().Length()));
}

void AInGameCharacter::BindCallbacksToDependencies()
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BindCallbacksToDependencies IsValid(AbilitySystemComponent) == false"));
		return;
	}
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BindCallbacksToDependencies IsValid(AttributeSet) == false"));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerHealth(Data.NewValue, AttributeSet->GetMaxHealth());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetAttackSpeedAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			AttackSpeed = Data.NewValue;
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerHunger(Data.NewValue, AttributeSet->GetMaxHunger());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerEnergy(Data.NewValue, AttributeSet->GetMaxEnergy());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerEnergy(Data.NewValue, AttributeSet->GetMaxEnergy());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetSpeedAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetJumpVelocityAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			GetCharacterMovement()->JumpZVelocity = Data.NewValue;
		}
	);
}

void AInGameCharacter::BroadcastInitialValues()
{
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BroadcastInitialValues IsValid(AttributeSet) == false"));
		return;
	}

	Client_OnUpdatePlayerHealth(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
	Client_OnUpdatePlayerHunger(AttributeSet->GetHunger(), AttributeSet->GetMaxHunger());
	Client_OnUpdatePlayerEnergy(AttributeSet->GetEnergy(), AttributeSet->GetMaxEnergy());
}

void AInGameCharacter::Client_BroadcastInitialValues_Implementation()
{
	BroadcastInitialValues();
}

float AInGameCharacter::GetAttackSpeed() const
{
	return AttackSpeed;
}

void AInGameCharacter::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	for (FGameplayEffectSpecHandle SpecHandle : GameplayEffectSpecHandles)
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

bool AInGameCharacter::bCanBeHit()
{
	return IsAlive();
}

void AInGameCharacter::OnHittableDestroyed_Implementation()
{
	AbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Explode"));
	AbilitySystemComponent->CancelAllAbilities();

	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	UnequipAllItems();
	bIsAlive = false;

	UnPossessed();
}

bool AInGameCharacter::HasEquipmentSlotWithTag(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (CheckParentTag)
	{
		return EquipmentSlots.Contains(SlotTag) || EquipmentSlots.Contains(SlotTag.RequestDirectParent());
	}
	
	return EquipmentSlots.Contains(SlotTag);
}

void AInGameCharacter::SetEquipedItemAtSlot_Implementation(FGameplayTag SlotTag, AItemBase* Item, bool CheckParentTag)
{
	if (HasEquipmentSlotWithTag(SlotTag, CheckParentTag) == false)
	{
		return;
	}

	if (HasEquipmentSlotWithTag(SlotTag, false) == false)
	{
		EquipmentSlots[SlotTag.RequestDirectParent()] = Item;
	}
	else
	{
		EquipmentSlots[SlotTag] = Item;
	}
	
	return;
}

AItemBase* AInGameCharacter::GetEquipedItemAtSlot(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (HasEquipmentSlotWithTag(SlotTag, CheckParentTag) == false)
	{
		return nullptr;
	}

	if (HasEquipmentSlotWithTag(SlotTag, false) == false)
	{
		return EquipmentSlots[SlotTag.RequestDirectParent()];
	}
	else
	{
		return EquipmentSlots[SlotTag];
	}
}

TArray<AItemBase*> AInGameCharacter::GetAllEquipedItems() const
{
	TArray<AItemBase*> EquipedItems;
	for (auto& EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
		{
			EquipedItems.Add(EquipmentSlot.Value);
		}
	}
	return EquipedItems;
}

void AInGameCharacter::UnequipAllItems()
{
	for (auto& EquipmentSlot : EquipmentSlots)
	{
		if (IsValid(EquipmentSlot.Value))
		{
			EquipmentSlot.Value->OnItemUnequiped();
			EquipmentSlot.Value = nullptr;
		}
	}
}

TArray<ADamageDealingItem*> AInGameCharacter::GetAllEquipedWeapons() const
{
	TArray<ADamageDealingItem*> EquipedWeapons;
	for (auto& EquipmentSlot : EquipmentSlots)
	{
		ADamageDealingItem* EquipedWeapon = Cast<ADamageDealingItem>(EquipmentSlot.Value);
		if (IsValid(EquipedWeapon))
		{
			EquipedWeapons.Add(EquipedWeapon);
		}
	}
	return EquipedWeapons;
}

bool AInGameCharacter::HasEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (CheckParentTag)
	{
		return EquipmentSockets.Contains(SlotTag) || EquipmentSockets.Contains(SlotTag.RequestDirectParent());
	}

	return EquipmentSockets.Contains(SlotTag);
}

FName AInGameCharacter::GetEquipmentSocketWithTag(FGameplayTag SlotTag, bool CheckParentTag) const
{
	if (HasEquipmentSocketWithTag(SlotTag, CheckParentTag) == false)
	{
		return FName();
	}

	if (HasEquipmentSocketWithTag(SlotTag, false) == false)
	{
		return EquipmentSockets[SlotTag.RequestDirectParent()];
	}
	else
	{
		return EquipmentSockets[SlotTag];
	}
}

bool AInGameCharacter::IsAlive()
{
	return bIsAlive;
}

void AInGameCharacter::ShowPlayerInfo()
{
	FString Prefix;
	if (HasAuthority())
	{
		Prefix = "Server:";
	}
	else if (IsLocallyControlled())
	{
		Prefix = "Owning Client:";
	}
	else
	{
		Prefix = "Client:";
	}

	UE_LOG(LogTemp, Warning, TEXT("%s ===== Character stats ====="), *Prefix);

	if (GetFocusedInteractableObject() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject is a valid pointer"), *Prefix);

		IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(GetFocusedInteractableObject());
		if (InteractableActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject implements IInteractableInterface"), *Prefix);
			if (InteractableActor->bCanInteract(GetController()))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject can be interacted with"), *Prefix);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject cannot be interacted with"), *Prefix);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s FocusedInteractableObject == nullptr"), *Prefix);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Player has %d equipment slots"), *Prefix, EquipmentSlots.Num());
	UE_LOG(LogTemp, Warning, TEXT("%s Player has %d equipment sockets"), *Prefix, EquipmentSockets.Num());

	int i = 0;
	for (TPair<FGameplayTag, AItemBase*> Slot : EquipmentSlots)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Slot #%d - %s"), *Prefix, i, *(Slot.Key.ToString()));
		if (IsValid(Slot.Value))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s has a valid item equiped"), *Prefix, *(Slot.Key.ToString()));
			if (Slot.Value->bCanInteract(GetController()))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s %s can be interacted with, THIS SHOULD NOT BE POSSIBLE"), *Prefix, *(Slot.Key.ToString()));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s %s cannot be interacted with as it is equiped by the player"), *Prefix, *(Slot.Key.ToString()));
			}
			UE_LOG(LogTemp, Warning, TEXT("%s %s is a %s"), *Prefix, *(Slot.Key.ToString()), *(Slot.Value->ItemData.ItemName.ToString()));
			UE_LOG(LogTemp, Warning, TEXT("%s %s Durability is %f out of %f - %f percent"), *Prefix, *(Slot.Key.ToString()), Slot.Value->GetDurability(), 
				Slot.Value->ItemData.ItemMaxDurability, Slot.Value->GetDurability() / Slot.Value->ItemData.ItemMaxDurability * 100);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s is nullptr"), *Prefix, *(Slot.Key.ToString()));
		}
		if (HasEquipmentSocketWithTag(Slot.Key))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s has equipment socket with name %s"), *Prefix, *(Slot.Key.ToString()), *(GetEquipmentSocketWithTag(Slot.Key).ToString()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s does not have an associated equipment socket!"), *Prefix, *(Slot.Key.ToString()));
		}

		i++;
	}
}
