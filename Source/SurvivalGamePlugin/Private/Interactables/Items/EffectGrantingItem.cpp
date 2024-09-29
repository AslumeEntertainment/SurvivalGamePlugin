// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/EffectGrantingItem.h"


TMap<TSubclassOf<UGameplayEffect>, uint8> AEffectGrantingItem::GetEffects()
{
	return Effects;
}
