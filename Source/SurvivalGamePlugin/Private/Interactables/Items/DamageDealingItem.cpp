// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/DamageDealingItem.h"
#include "Characters/SG_Character.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "GAS/SG_AttributeSet.h"


void ADamageDealingItem::OnItemEquiped(ASG_Character* PlayerOwner, FName SocketName)
{
    Super::OnItemEquiped(PlayerOwner, SocketName);

    if (SocketName == FName("EquipmentSocket.Right"))
    {
        PlayerOwner->GetAbilitySystemComponent()->SetNumericAttributeBase(USG_AttributeSet::GetDamageRightAttribute(), Damage);
    }
    else if (SocketName == FName("EquipmentSocket.Left"))
    {
        PlayerOwner->GetAbilitySystemComponent()->SetNumericAttributeBase(USG_AttributeSet::GetDamageLeftAttribute(), Damage);
    }
}

void ADamageDealingItem::OnItemUnequiped()
{
    FName SocketName = GetAttachParentSocketName();
    ASG_Character* PlayerOwner = Cast<ASG_Character>(GetAttachParentActor());
    if (!IsValid(PlayerOwner))
    {
        UE_LOG(LogTemp, Error, TEXT("ADamageDealingItem::OnItemUnequiped IsValid(PlayerOwner) == false"));
        return;
    }

    if (SocketName == FName("EquipmentSocket.Right"))
    {
        PlayerOwner->GetAbilitySystemComponent()->SetNumericAttributeBase(USG_AttributeSet::GetDamageRightAttribute(), PlayerOwner->GetBaseDamage());
    }
    else if (GetAttachParentSocketName() == FName("EquipmentSocket.Left"))
    {
        PlayerOwner->GetAbilitySystemComponent()->SetNumericAttributeBase(USG_AttributeSet::GetDamageLeftAttribute(), PlayerOwner->GetBaseDamage());
    }

    Super::OnItemUnequiped();
}

float ADamageDealingItem::GetItemDamage()
{
    return Damage;
}
