// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Headers/ButtonDelegates.h"

#include "LeaveGameWidgetBase.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class SURVIVALGAMEPLUGIN_API ULeaveGameWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnExitClicked();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* ExitButton;

public:

	virtual void NativeOnInitialized() override;

	FButtonClicked ExitButtonClicked;

};
