// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GeneralDelegates.generated.h"

//------------------------------------Primitive Types---------------------------------------
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVoidDataTransferSignature);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntegerTransferSignature, int, Num);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBooleanTransferSignature, bool, State);

//------------------------------------------------------------------------------------------




//---------------------------------------Engine Structs-------------------------------------
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextTransferSignature, FText, Text);

//- - - - - - - - - - Arrays - - - - - 
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStringArrayTransferSignature, TArray<FString>, FriendNames);

//------------------------------------------------------------------------------------------




//--------------------------------------Engine Classes-------------------------------------
UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerControllerTransferSignature, APlayerController*, TriggeringPlayer);

//-----------------------------------------------------------------------------------------
