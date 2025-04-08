// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Items/ConsumableItem.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AConsumableItem::AConsumableItem()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimulusSource"));
}
