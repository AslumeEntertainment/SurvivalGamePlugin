// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ButtonDelegates.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonClicked);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIconClicked, FName, IconName);