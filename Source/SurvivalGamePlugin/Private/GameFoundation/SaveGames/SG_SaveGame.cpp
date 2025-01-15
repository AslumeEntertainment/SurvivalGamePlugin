// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFoundation/SaveGames/SG_SaveGame.h"

void USG_SaveGame::Init()
{
	ResourceCards.Init(FProgressiveDataHelperStruct(), MaxResourceCards);
	HeroCards.Init(FProgressiveDataHelperStruct(), MaxHeroCards);
	Decks.Init(FDeckPersistentData(), MaxDecks);
}
