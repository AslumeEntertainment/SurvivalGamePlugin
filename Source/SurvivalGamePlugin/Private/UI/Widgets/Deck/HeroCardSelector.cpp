// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Deck/HeroCardSelector.h"

#include "Components/Image.h"

void UHeroCardSelector::SetHeroBackgroundImage(UTexture2D* ItemImage)
{
	IMG_HeroBackgroundImage->SetBrushFromTexture(ItemImage);
}
