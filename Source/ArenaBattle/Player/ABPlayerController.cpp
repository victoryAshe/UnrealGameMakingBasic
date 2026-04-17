// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드 설정.
	// 게임 시작되면 뷰포트로 바로 입력 되도록.
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}