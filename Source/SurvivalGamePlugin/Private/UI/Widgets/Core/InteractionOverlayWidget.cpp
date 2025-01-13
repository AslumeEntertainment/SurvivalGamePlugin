// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Core/InteractionOverlayWidget.h"

#include "Components/TextBlock.h"

void UInteractionOverlayWidget::SetInteractIndicationText(FText InteractIndicationText)
{
	TXT_InteractIndicationText->SetText(InteractIndicationText);
}
