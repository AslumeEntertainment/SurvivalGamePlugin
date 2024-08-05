// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/DamageDealingItem.h"


void ADamageDealingItem::OnItemEquiped(ASG_Character* NewPlayerOwner, FName SocketName)
{
    Super::OnItemEquiped(NewPlayerOwner, SocketName);
}

void ADamageDealingItem::OnItemUnequiped()
{
    Super::OnItemUnequiped();
}

float ADamageDealingItem::GetItemDamage()
{
    return Damage;
}
