// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_AttemptDisassembleItem.h"
#include "Characters/HumanCharacter.h"
#include "Interactables/Items/ItemBase.h"
#include "Animation/SG_AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
//#include "GAS/AbilityTasks/SGAT_PlayMontageAndWaitForEvent.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"


void USGGA_AttemptDisassembleItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USGGA_AttemptDisassembleItem, MainItemActor);
	DOREPLIFETIME(USGGA_AttemptDisassembleItem, IngredientItemActor);
	DOREPLIFETIME(USGGA_AttemptDisassembleItem, MainItemSocket);
	DOREPLIFETIME(USGGA_AttemptDisassembleItem, IngredientItemSocket);
}

void USGGA_AttemptDisassembleItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	Player = Cast<AHumanCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AnimInst = Cast<USG_AnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!IsValid(AnimInst))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::ActivateAbility IsValid(AnimInst) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	if (bCanDisassembleItem(Player->MainHandItem, Player->SecondaryHandItem))
	{
		ItemForDisassembly = &Player->MainHandItem;
		OtherItem = &Player->SecondaryHandItem;
		MainItemSocket = Player->GetMainHandSocket();
		IngredientItemSocket = Player->GetSecondaryHandSocket();
	}
	else if (bCanDisassembleItem(Player->SecondaryHandItem, Player->MainHandItem))
	{
		ItemForDisassembly = &Player->SecondaryHandItem;
		OtherItem = &Player->MainHandItem;
		MainItemSocket = Player->GetSecondaryHandSocket();
		IngredientItemSocket = Player->GetMainHandSocket();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Disassembling: No item can be disassembled!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (MainItemSocket == Player->GetLeftHandSocket())
	{
		AnimInst->bIsMirrored = true;
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnNewItems();
	}

	float NewSpeed = CurrentActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute()) * ItemForDisassemblyData->MovementSpeedMultiplier;
	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute(), NewSpeed);

	/*FGameplayTagContainer EventTagContainer = FGameplayTagContainer();
	EventTagContainer.AddTag(FGameplayTag::RequestGameplayTag("Notify.FinishDisassembly"));

	USGAT_PlayMontageAndWaitForEvent* Task = USGAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, ItemForDisassemblyData->Animation, EventTagContainer, 1.0f, NAME_None, false, 1.0f);
	Task->OnCompleted.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageCompleted);
	Task->OnCancelled.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageCanceled);
	Task->EventReceived.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnDisassemblyFinished);
	Task->ReadyForActivation();*/

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ItemForDisassemblyData->Animation);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnAnimMontageCanceled);
	PlayMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Notify.FinishDisassembly"), nullptr, true, true);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USGGA_AttemptDisassembleItem::OnDisassemblyFinished);
	WaitGameplayEventTask->Activate();
}

bool USGGA_AttemptDisassembleItem::bCanDisassembleItem(AItemBase* MainItemForDisassembly, AItemBase* OtherEquipedItem)
{
	if (!IsValid(MainItemForDisassembly))
	{
		UE_LOG(LogTemp, Warning, TEXT("Disassembling: Current item is not valid!"));
		return false;
	}

	FDisassemblyProcessDataStruct* ItemData = DisassemlyInfoTable->FindRow<FDisassemblyProcessDataStruct>(MainItemForDisassembly->GetItemDataRowName(), "");
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::AttemptDisassembleItem ItemData == nullptr"));
		return false;
	}

	switch (ItemData->DisassembleRequirement)
	{
		case EItemDisassembleRequirement::Unable:
		{
			UE_LOG(LogTemp, Warning, TEXT("Disassembling: Current item cannot be disassembled!"));
			return false;
		}
		case EItemDisassembleRequirement::Always:
		{
			break;
		}
		case EItemDisassembleRequirement::EmptyHand:
		{
			if (IsValid(OtherEquipedItem))
			{
				UE_LOG(LogTemp, Warning, TEXT("Disassembling: The player needs an empty hand!"));
				return false;
			}
			break;
		}
	}

	ItemForDisassemblyData = ItemData;
	return true;
}

void USGGA_AttemptDisassembleItem::SpawnNewItems()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_AttemptDisassembleItem::SpawnNewItems IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	MainItemActor = World->SpawnActor<AItemBase>(ItemForDisassemblyData->MainParentItem, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(MainItemActor))
	{
		MainItemActor->SetActorScale3D(FVector(0.0001, 0.0001, 0.0001));
	}
	IngredientItemActor = World->SpawnActor<AItemBase>(ItemForDisassemblyData->IngredientParentItem, Player->GetPlayerLookAtLocation(100.f), Player->GetActorRotation());
	if (IsValid(IngredientItemActor))
	{
		IngredientItemActor->SetActorScale3D(FVector(0.0001, 0.0001, 0.0001));
	}

	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::SpawnNewItems - New items have been spawned and kept hidden until ability has finished"));
}

void USGGA_AttemptDisassembleItem::OnAnimMontageCompleted(/*FGameplayTag EventTag, FGameplayEventData Payload*/)
{
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: Ability has been completed, resetting speed"));

	float OldSpeed = CurrentActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute()) / ItemForDisassemblyData->MovementSpeedMultiplier;
	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute(), OldSpeed);

	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USGGA_AttemptDisassembleItem::OnAnimMontageCanceled(/*FGameplayTag EventTag, FGameplayEventData Payload*/)
{
	UE_LOG(LogTemp, Warning, TEXT("Disassembly: Ability has been canceled, resetting speed"));

	float OldSpeed = CurrentActorInfo->AbilitySystemComponent->GetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute()) / ItemForDisassemblyData->MovementSpeedMultiplier;
	CurrentActorInfo->AbilitySystemComponent->SetNumericAttributeBase(USG_AttributeSet::GetSpeedAttribute(), OldSpeed);

	AnimInst->bIsMirrored = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_AttemptDisassembleItem::OnDisassemblyFinished(/*FGameplayTag EventTag, */FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::OnDisassemblyFinished - Gameplay Event recieved"));
	(*ItemForDisassembly)->DestroyItem();

	if (IsValid(MainItemActor))
	{
		MainItemActor->OnItemEquiped(Player, MainItemSocket);
		//MainItemActor->SetActorScale3D(FVector(1, 1, 1));
		(*ItemForDisassembly) = MainItemActor;
	}
	if (!IsValid(*OtherItem) && IsValid(IngredientItemActor))
	{
		IngredientItemActor->OnItemEquiped(Player, IngredientItemSocket);
		//IngredientItemActor->SetActorScale3D(FVector(1, 1, 1));
		*OtherItem = IngredientItemActor;
	}
	else if (IsValid(*OtherItem) && IsValid(IngredientItemActor))
	{
		IngredientItemActor->SetActorLocation(Player->GetPlayerLookAtLocation(100.f));
		IngredientItemActor->SetActorRotation(Player->GetActorRotation());
		//IngredientItemActor->SetActorScale3D(FVector(1, 1, 1));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("USGGA_AttemptDisassembleItem::OnDisassemblyFinished - Disassembly process finished!"));
}
