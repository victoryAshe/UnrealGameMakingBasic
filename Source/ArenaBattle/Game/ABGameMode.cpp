// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "Player/ABPlayerController.h"
#include "Character/ABCharacterPlayer.h"

AABGameMode::AABGameMode()
{

	// 기본 폰 클래스 지정.
	DefaultPawnClass = AABCharacterPlayer::StaticClass();

	// 기본 클래스 지정.
	PlayerControllerClass = AABPlayerController::StaticClass();

}