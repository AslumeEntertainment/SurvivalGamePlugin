// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GeneralDelegates.generated.h"


//------------------------------------Primitive Types---------------------------------------
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVoidDataTransferSignature);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntegerTransferSignature, int, Num);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatTransferSignature, float, Num);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBooleanTransferSignature, bool, State);

//------------------------------------------------------------------------------------------




//---------------------------------------Engine Structs-------------------------------------
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextTransferSignature, FText, Text);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNameTransferSignature, FName, Name);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStringTransferSignature, FString, Text);

//- - - - - - - - - - Arrays - - - - - 
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStringArrayTransferSignature, TArray<FString>, FriendNames);

//------------------------------------------------------------------------------------------




//--------------------------------------Engine Classes-------------------------------------
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerControllerTransferSignature, APlayerController*, TriggeringPlayer);


UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetTransferSignature, UUserWidget*, Widget);

class UPanelWidget;
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPanelWidgetTransferSignature, UPanelWidget*, Widget);

class USaveGame;
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSaveGameTransferSignature, USaveGame*, ReadGame);

//-----------------------------------------------------------------------------------------



//----------------------------------------Deck Related-------------------------------------

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FOnCardDroppedSignature, UCardSlotWidget*, Card, int, CardIndex);

//-----------------------------------------------------------------------------------------