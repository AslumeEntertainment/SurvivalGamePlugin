// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardContainer.generated.h"

class UWrapBox;
class UWidgetSwitcher;
class UUnitIconWidget;

UCLASS()
class SURVIVALGAMEPLUGIN_API UCardContainer : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* CardWrapBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWidgetSwitcher* CardWidgetSwitcher;

	UPROPERTY(EditAnywhere, Category = "CardWidget")
	TSubclassOf<UUnitIconWidget> CardWidgetClass;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:

	TArray<UUnitIconWidget*> GetCards();

	UFUNCTION()
	UUnitIconWidget* AddCard(FText Name, UTexture2D* Image, FLinearColor Color);

	UFUNCTION()
	void ClearCards();

};
