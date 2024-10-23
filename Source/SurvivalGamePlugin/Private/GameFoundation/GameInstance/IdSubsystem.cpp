// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/GameInstance/IdSubsystem.h"

void UIdSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UIdSubsystem::Deinitialize()
{
}

bool UIdSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	return false;
}

FDataTableRowHandle UIdSubsystem::GetDataRowFromID(uint16 ID)
{
	FDataTableRowHandle* RowPtr = IDMap.Find(ID);
	if (RowPtr == nullptr)
	{
		return FDataTableRowHandle();
	}
	return *RowPtr;
}

uint16 UIdSubsystem::GetIdFromDataRow(FDataTableRowHandle RowHandle)
{
	const uint16* IDPtr = IDMap.FindKey(RowHandle);
	if (IDPtr == nullptr)
	{
		return 0;
	}
	return *IDPtr;
}

