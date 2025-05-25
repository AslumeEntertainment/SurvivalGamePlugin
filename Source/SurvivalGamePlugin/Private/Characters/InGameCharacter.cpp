// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/InGameCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/SG_AIPerceptionStimuliSourceComp.h"
#include "Components/DamageDealingComponent.h"
#include "Components/LootTableComponent.h"
#include "Components/EquipmentComponent.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Headers/AbilityInputConfig.h"
#include "UI/HUD/InGameHUD.h"

#include "Level/WaterVolume.h"
#include "Landscape.h"

AInGameCharacter::AInGameCharacter()
{
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

	LootTableComponent = CreateDefaultSubobject<ULootTableComponent>(TEXT("LootTable"));
	
	ImpactChecker = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ImpactCheker"));
	ImpactChecker->SetupAttachment(GetCapsuleComponent());
	ImpactChecker->OnComponentBeginOverlap.AddDynamic(this, &AInGameCharacter::OnComponentBeginTouch);

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AInGameCharacter::OnComponentHitReceived);
	GetCapsuleComponent()->SetUseCCD(false);

	StimulusSource = CreateDefaultSubobject<USG_AIPerceptionStimuliSourceComp>(TEXT("StimulusSource"));

	OnCharacterDeath.AddDynamic(this, &AInGameCharacter::UnbindOverlayDelegates);
	OnDestroyed.AddDynamic(this, &AInGameCharacter::RemoveIngameOverlay);

}

void AInGameCharacter::RemoveIngameOverlay(AActor* Self)
{
	Client_SetInGameOverlayVisibility(false);
}

void AInGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AInGameCharacter, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AInGameCharacter, bIsAlive, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AInGameCharacter, bIsExhausted, COND_None, REPNOTIFY_OnChanged);
}

void AInGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<UDamageDealingComponent*> DamageDealingComponents;
	GetComponents<UDamageDealingComponent>(DamageDealingComponents);

	for (UDamageDealingComponent* DamageSource : DamageDealingComponents)
	{
		AddDamageSource(DamageSource);
		DamageSource->ConfigDamageSource(this);
	}

	SetGenericTeamId(TeamId);

	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController) == false)
		{
			return;
		}
		AInGameHUD* HUD = Cast<AInGameHUD>(PlayerController->GetHUD());
		if (IsValid(HUD) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BeginPlay IsValid(HUD) == false"));
			return;
		}
		OnCharacterDeath.AddDynamic(HUD, &AInGameHUD::RemoveInteractionOverlayWidget);
	}
}

void AInGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		Client_OnUpdatePlayerCurrentSpeed(GetVelocity().Length());
	}
}

void AInGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BindCallbacksToDependencies();

	if (IsValid(Cast<APlayerController>(GetController())))
	{
		Client_SetInGameOverlayVisibility(true);
	}
}

void AInGameCharacter::Client_OnUpdatePlayerHealth_Implementation(float CurrentHealth, float MaxHealth)
{
	OnHealthPercentUpdated.Broadcast(CurrentHealth / MaxHealth);
}

void AInGameCharacter::Client_OnUpdatePlayerMaxHealth_Implementation(float MaxHealth)
{
	OnMaxHealthUpdated.Broadcast(MaxHealth);
}

void AInGameCharacter::Client_OnUpdatePlayerHunger_Implementation(float CurrentHunger, float MaxHunger)
{
	OnHungerPercentUpdated.Broadcast(CurrentHunger / MaxHunger);
}

void AInGameCharacter::Client_OnUpdatePlayerMaxHunger_Implementation(float MaxHunger)
{
	OnMaxHungerUpdated.Broadcast(MaxHunger);
}

void AInGameCharacter::Client_OnUpdatePlayerEnergy_Implementation(float CurrentEnergy, float MaxEnergy)
{
	if (CurrentEnergy == MaxEnergy) SetIsExhausted(false);
	if (CurrentEnergy == 0) SetIsExhausted(true);
	OnEnergyPercentUpdated.Broadcast(CurrentEnergy / MaxEnergy);
}

void AInGameCharacter::Client_OnUpdatePlayerMaxEnergy_Implementation(float MaxEnergy)
{
	OnMaxEnergyUpdated.Broadcast(MaxEnergy);
}

void AInGameCharacter::Client_OnUpdatePlayerCurrentSpeed_Implementation(float Velocity)
{
	OnCurrentSpeedUpdated.Broadcast(Velocity);
}

void AInGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (bIsRightHanded)
		{
			EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::MainHandAction);
			EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::SecondaryHandAction);

			EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Completed, this, &AInGameCharacter::CancelMainHandAction);
			EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Completed, this, &AInGameCharacter::CancelSecondaryHandAction);
		}
		else
		{
			EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::SecondaryHandAction);
			EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Triggered, this, &AInGameCharacter::MainHandAction);
		
			EnhancedInputComponent->BindAction(UseMainHandItemAction, ETriggerEvent::Completed, this, &AInGameCharacter::CancelSecondaryHandAction);
			EnhancedInputComponent->BindAction(UseSecondaryHandItemAction, ETriggerEvent::Completed, this, &AInGameCharacter::CancelMainHandAction);
		}
	}

	else
	{
		UE_LOG(LogSGCharacter, Error, TEXT("AHumanCharacter::SetupPlayerInputComponent Failed to find an Enhanced Input component!"));
	}
}

UShapeComponent* AInGameCharacter::GetImpactChecker()
{
	return ImpactChecker;
}

void AInGameCharacter::OnComponentBeginTouch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsAlive() == false) return;

	if (IsValid(Cast<AWaterVolume>(OtherActor)) == true)
	{
		return;
	}

	bool isFalling = (GetVelocity().GetSafeNormal().Z < -0.5);
	if (IsValid(Cast<ALandscape>(OtherActor)) == true && !isFalling)
	{
		return;
	}

	IHittable* Hit = Cast<IHittable>(OtherActor);
	bool TouchingHittable = Hit == nullptr;
	
	//UE_LOG(LogTemp, Error, TEXT("%s: Ovelapping with %s (comp: %s) i toi e hittable(%d)"), *GetName(), *OtherActor->GetName(), *OtherComp->GetName(), TouchingHittable);

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("CommonEvents.ReceiveDamage.Impact");

	FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());

	ContextHandle.AddInstigator(OtherActor, OtherActor);

	FGameplayEventData EventData = FGameplayEventData();
	EventData.Instigator = OtherActor;
	EventData.ContextHandle = ContextHandle;

	USG_AbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

void AInGameCharacter::OnComponentHitReceived(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/*
	UMeshComponent* OtherMesh = Cast<UMeshComponent>(OtherComp);

	FVector OtherActorMaxLocalBounds = OtherComp->GetLocalBounds().GetBox().Max;
	FVector OtherActorTop = OtherActor->GetActorLocation() + OtherActor->GetActorUpVector() * OtherActorMaxLocalBounds.Z;

	FVector SelfOrigin;
	FVector SelfBoxExtent;
	GetActorBounds(true, SelfOrigin, SelfBoxExtent);

	float MaxAcceptableHeightZ = (SelfOrigin.Z - SelfBoxExtent.Z) + GetCharacterMovement()->MaxStepHeight;

	if (bShouldDrawDebug)
	{
		UE_LOG(LogSGCharacter, Error, TEXT("Actor Top - %.2f"), OtherActorTop.Z);
		UE_LOG(LogSGCharacter, Error, TEXT("Max acceptable height - %.2f"), MaxAcceptableHeightZ);
	}

	if (OtherActorTop.Z <= MaxAcceptableHeightZ)
	{
		return;
	}
	*/

}

void AInGameCharacter::Jump()
{
	if (IsMovementEnabled() == false)
	{
		return;
	}

	Super::Jump();
}

void AInGameCharacter::Landed(const FHitResult& Hit)
{
	/*Super::Landed(Hit);

	FVector NormalVelocity = UKismetMathLibrary::Normal(GetVelocity());
	if (NormalVelocity.Z > -0.45)
	{
		return;
	}

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("CommonEvents.ReceiveDamage.Fall");

	FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
	ContextHandle.AddHitResult(Hit);
	
	FGameplayEventData EventData = FGameplayEventData();
	EventData.ContextHandle = ContextHandle;

	SendGameplayEventToSelf(EventTag, EventData);*/
}

void AInGameCharacter::MainHandAction()
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		FGameplayTag EventTag;
		FGameplayEventData EventData;

		if (EquipmentComponent->HasEquipmentSlotWithTag(EquipmentComponent->GetMixedEquipmentSlotTag()))
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Mixed");
		}
		else
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Main");
		}

		SendGameplayEventToSelf(EventTag, EventData);
		return;
	}

	FGameplayTagContainer MainAbilityTagContainer;
	if (IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMixedEquipmentSlotTag())))
	{
		MainAbilityTagContainer = EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMixedEquipmentSlotTag())->GetItemAbilityTags();
	}
	else if (IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMainEquipmentSlotTag())))
	{
		MainAbilityTagContainer = EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMainEquipmentSlotTag())->GetItemAbilityTags();
	}
	else
	{
		MainAbilityTagContainer = MainHandAbility.GetDefaultObject()->AbilityTags;
	}

	if (IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetSecondaryEquipmentSlotTag())))
	{
		AItemBase* ItemRef = EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetSecondaryEquipmentSlotTag());

		float CurrentStrength = AbilitySystemComponent->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentStrengthAttribute());
		EWeight ItemWeight = ItemRef->ItemData.ItemWeight;

		if ((uint8)ItemWeight > CurrentStrength)
		{
			MainAbilityTagContainer = ItemRef->GetItemSecondaryAbilityTags();
		}
	}
	
	AbilitySystemComponent->TryActivateAbilitiesByTag(MainAbilityTagContainer);
}

void AInGameCharacter::CancelMainHandAction()
{
	if (bShouldDrawDebug) UE_LOG(LogTemp, Log, TEXT("AInGameCharacter::CancelMainHandAction"));
}

void AInGameCharacter::SecondaryHandAction()
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.IsUnequiping")))
	{
		FGameplayTag EventTag;
		FGameplayEventData EventData;

		if (EquipmentComponent->HasEquipmentSlotWithTag(EquipmentComponent->GetMixedEquipmentSlotTag()))
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Mixed");
		}
		else
		{
			EventTag = FGameplayTag::RequestGameplayTag("PlayerAction.Unequip.Secondary");
		}

		SendGameplayEventToSelf(EventTag, EventData);
		return;
	}

	FGameplayTagContainer SecondaryAbilityTagContainer;
	if (IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMixedEquipmentSlotTag())))
	{
		SecondaryAbilityTagContainer = EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMixedEquipmentSlotTag())->GetItemSecondaryAbilityTags();
	}
	else if (IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetSecondaryEquipmentSlotTag())))
	{
		SecondaryAbilityTagContainer = EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetSecondaryEquipmentSlotTag())->GetItemAbilityTags();
	}
	else
	{
		SecondaryAbilityTagContainer = SecondaryHandAbility.GetDefaultObject()->AbilityTags;
	}

	if (IsValid(EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMainEquipmentSlotTag())))
	{
		AItemBase* ItemRef = EquipmentComponent->GetEquipedItemAtSlot(EquipmentComponent->GetMainEquipmentSlotTag());

		float CurrentStrength = AbilitySystemComponent->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentStrengthAttribute());
		EWeight ItemWeight = ItemRef->ItemData.ItemWeight;

		if ((uint8)ItemWeight > CurrentStrength)
		{
			SecondaryAbilityTagContainer = ItemRef->GetItemSecondaryAbilityTags();
		}
	}
	
	AbilitySystemComponent->TryActivateAbilitiesByTag(SecondaryAbilityTagContainer);
}

void AInGameCharacter::CancelSecondaryHandAction()
{
	if(bShouldDrawDebug) UE_LOG(LogTemp, Log, TEXT("AInGameCharacter::CancelSecondaryHandAction"));
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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerHealth(Data.NewValue, AttributeSet->GetMaxHealth());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerMaxHealth(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerHunger(Data.NewValue, AttributeSet->GetMaxHunger());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerMaxHunger(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerEnergy(Data.NewValue, AttributeSet->GetMaxEnergy());
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			Client_OnUpdatePlayerMaxEnergy(Data.NewValue);
		}
	);
}

void AInGameCharacter::InitCharacterDefaults()
{
	Super::InitCharacterDefaults();

	AbilitySystemComponent->AddAbility(MainHandAbility, 1, this);
	AbilitySystemComponent->AddAbility(SecondaryHandAbility, 1, this);
}

void AInGameCharacter::OnRep_IsAlive()
{
	if (bIsAlive == false) OnCharacterDeath.Broadcast();
}

void AInGameCharacter::BroadcastInitialValues()
{
	if (HasAuthority() == false)
	{
		FTimerHandle BroadcastInitialValuesTimerHandle;
		GetWorldTimerManager().SetTimer(BroadcastInitialValuesTimerHandle, this, &AInGameCharacter::Client_BroadcastInitialValues, 0.5f, false);
	}
	
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::BroadcastInitialValues IsValid(AttributeSet) == false"));
		return;
	}

	Client_OnUpdatePlayerHealth(AttributeSet->GetCurrentHealth(), AttributeSet->GetMaxHealth());
	Client_OnUpdatePlayerMaxHealth(AttributeSet->GetMaxHealth());
	Client_OnUpdatePlayerHunger(AttributeSet->GetHunger(), AttributeSet->GetMaxHunger());
	Client_OnUpdatePlayerMaxHunger(AttributeSet->GetMaxHunger());
	Client_OnUpdatePlayerEnergy(AttributeSet->GetEnergy(), AttributeSet->GetMaxEnergy());
	Client_OnUpdatePlayerMaxEnergy(AttributeSet->GetMaxEnergy());
}

void AInGameCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
}

void AInGameCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
}

void AInGameCharacter::AddDamageSource(UDamageDealingComponent* NewDamageSource)
{
	if (DamageSources.Contains(NewDamageSource->GetDamageSourceTag()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s already has a damage source with tag - %s"), *GetName(), *NewDamageSource->GetDamageSourceTag().ToString());
		return;
	}
	DamageSources.Add(NewDamageSource->GetDamageSourceTag(), NewDamageSource);
}

void AInGameCharacter::RemoveDamageSourceByTag(FGameplayTag DamageSourceTag)
{
	DamageSources.Remove(DamageSourceTag);
}

void AInGameCharacter::SetDamageSourceActivenessByTag(FGameplayTag DamageSourceTag, bool ActivenessState)
{
	UDamageDealingComponent* DamageSource = DamageSources.FindRef(DamageSourceTag);
	if (IsValid(DamageSource) == false)
	{
		return;
	}
	DamageSource->SetActiveness(ActivenessState);
}

void AInGameCharacter::SetActivenessForAllDamageSources(bool ActivenessState)
{
	for (TPair<FGameplayTag, UDamageDealingComponent*> DamageSource : DamageSources)
	{
		DamageSource.Value->SetActiveness(ActivenessState);
	}
}

void AInGameCharacter::Client_BroadcastOnCharacterDeath_Implementation()
{
	OnCharacterDeath.Broadcast();
}

void AInGameCharacter::Client_BroadcastInitialValues_Implementation()
{
	BroadcastInitialValues();
}

void AInGameCharacter::Client_SetInGameOverlayVisibility_Implementation(bool bShouldAddToViewport)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::Client_SetInGameOverlayVisibility_Implementation IsValid(InteractingPlayer) == false"));
		return;
	}
	AInGameHUD* HUD = PC->GetHUD<AInGameHUD>();
	if (IsValid(HUD) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::Client_SetInGameOverlayVisibility_Implementation IsValid(HUD) == false"));
		return;
	}

	bShouldAddToViewport ? HUD->ShowGameplayOverlayWidget(this) : HUD->RemoveGameplayOverlayWidget();
}

void AInGameCharacter::SetAttackSpeed(float NewValue)
{
	AttackSpeed = NewValue;
}

float AInGameCharacter::GetAttackSpeed(AItemBase* HeldItem) const
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::GetAttackSpeed IsValid(AbilitySystemComponent) == false"));
		return 1;
	}
	
	float AttributeMultiplier = AbilitySystemComponent->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentAttackSpeedPercentAttribute()) / 100;

	float FinalAttackSpeed = (AttributeMultiplier) * (IsExhausted() ? 0.75f : 1);

	return FinalAttackSpeed;
}

void AInGameCharacter::SetIsRightHanded(bool NewValue)
{
	bIsRightHanded = NewValue;
}

bool AInGameCharacter::IsRightHanded() const
{
	return bIsRightHanded;
}

void AInGameCharacter::SetIsAlive(bool NewValue)
{
	bIsAlive = NewValue;
	if(bIsAlive == false) OnCharacterDeath.Broadcast();
}

bool AInGameCharacter::IsAlive() const
{
	return bIsAlive;
}

void AInGameCharacter::SetIsExhausted(bool NewValue)
{
	bIsExhausted = NewValue;
}

bool AInGameCharacter::IsExhausted() const
{
	return bIsExhausted;
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

	UE_LOG(LogTemp, Warning, TEXT("%s Player has %d equipment slots"), *Prefix, EquipmentComponent->GetEquipmentSlots().Num());
	UE_LOG(LogTemp, Warning, TEXT("%s Player has %d equipment sockets"), *Prefix, EquipmentComponent->GetEquipmentSockets().Num());

	int i = 0;
	for (TPair<FGameplayTag, AItemBase*> Slot : EquipmentComponent->GetEquipmentSlots())
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
		if (EquipmentComponent->HasEquipmentSocketWithTag(Slot.Key))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s has equipment socket with name %s"), *Prefix, *(Slot.Key.ToString()), *(EquipmentComponent->GetEquipmentSocketWithTag(Slot.Key).ToString()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s does not have an associated equipment socket!"), *Prefix, *(Slot.Key.ToString()));
		}

		i++;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s Player possesses %d damage sources"), *Prefix, DamageSources.Num());

	i = 0;
	for (TPair<FGameplayTag, UDamageDealingComponent*> DamageSource : DamageSources)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Damage Source #%d - %s"), *Prefix, i, *DamageSource.Value->GetDamageSourceTag().ToString());
		if (IsValid(DamageSource.Value))
		{
			if (IsValid(DamageSource.Value->GetOwner()))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s Damage Source owner actor - %s"), *Prefix, *DamageSource.Value->GetOwner()->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s Damage Source doesn't have a valid owner actor!"), *Prefix);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Damage Source is invalid!"), *Prefix);
		}

		i++;
	}
}

bool AInGameCharacter::IsHitGuarded(FVector HitLocation) const
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Bool.HasState.Guard"))) == false)
	{
		return false;
	}

	FVector CharacterCenterPoint = GetMesh()->GetLocalBounds().GetBox().GetCenter();

	UE_LOG(LogTemp, Warning, TEXT("Hit point - %s; Target center - %s"), *HitLocation.ToString(), *CharacterCenterPoint.ToString());

	if (CharacterCenterPoint.Z > HitLocation.Z)
	{
		UE_LOG(LogTemp, Warning, TEXT("The hit is below the blocking range"));
		return false;
	}

	FVector TargetForwardVector = GetMesh()->GetForwardVector();
	FVector HitDirectionVector = UKismetMathLibrary::Normal(HitLocation - GetActorLocation());

	float DotProduct = UKismetMathLibrary::Dot_VectorVector(TargetForwardVector, HitDirectionVector);
	UE_LOG(LogTemp, Warning, TEXT("DotProduct - %f"), DotProduct);

	return (DotProduct > 0);
}

void AInGameCharacter::DestroyHittable(bool bSafeDestruct)
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AInGameCharacter::DestroyHittable IsValid(AbilitySystemComponent) == false"));
		return;
	}
	AbilitySystemComponent->OnOwnerDestroyed();

	if (HasAuthority() == false)
	{
		return;
	}

	SetIsAlive(false);

	if (HasAuthority() == false)
	{
		Client_BroadcastOnCharacterDeath();
	}

	EquipmentComponent->UnequipAllItems();
	LootTableComponent->DropLoot();
	Destroy();
}

USceneComponent* AInGameCharacter::GetBoundsComponent()
{
	return ImpactChecker;
}


