// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragWidget.generated.h"


class UUserWidget;
class UCardSlotWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UDragWidget : public UDragDropOperation
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* WidgetReference = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardSlotWidget* WidgetSlotReference = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ChildIndexReference = 0;
};
