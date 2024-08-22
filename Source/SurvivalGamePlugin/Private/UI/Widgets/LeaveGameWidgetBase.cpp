// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/LeaveGameWidgetBase.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "GameFoundation/PlayerControllers/SG_PlayerController.h"

void ULeaveGameWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ExitButton->OnClicked.AddDynamic(this, &ULeaveGameWidgetBase::OnExitClicked);
}

void ULeaveGameWidgetBase::OnExitClicked()
{

	ASG_PlayerController* PC = Cast<ASG_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	PC->Client_LeaveSession(true);
}

