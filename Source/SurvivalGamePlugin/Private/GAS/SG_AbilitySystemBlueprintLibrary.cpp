// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SG_AbilitySystemBlueprintLibrary.h"

#include "Kismet/KismetMathLibrary.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"


//void USG_AbilitySystemBlueprintLibrary::Server_SG_SendGameplayEventToActor(AActor* Actor, FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly)
//{
//	SG_SendGameplayEventToActor(Actor, EventTag, EventData, bServerOnly);
//}
//
//void USG_AbilitySystemBlueprintLibrary::SG_SendGameplayEventToActor(AActor* Actor, FGameplayTag EventTag, FGameplayEventData EventData, bool bServerOnly)
//{
//	if (Actor->HasAuthority() == false)
//	{
//		Server_SG_SendGameplayEventToActor(Actor, EventTag, EventData, bServerOnly);
//		if (bServerOnly) return;
//	}
//
//	UAbilitySystemComponent* ASC = GetAbilitySystemComponent(Actor);
//	ASC->HandleGameplayEvent(EventTag, &EventData);
//}
