// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
//#include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
	// 블루프린트 클래스를 애셋 로드하듯이..
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C")
	);

	// 검색 성공 여부 확인.
	if (PawnClassRef.Succeeded())
	{
		// 기본 폰 클래스 설정.
		DefaultPawnClass = PawnClassRef.Class;
	}

	// 기본 클래스 지정.
	//PlayerControllerClass = AABPlayerController::StaticClass();

	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(
	//	TEXT("/Script/ArenaBattle.ABPlayerController")
	//);

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController.BP_ThirdPersonPlayerController_C")
	);

	if (PlayerControllerRef.Succeeded())
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}
}