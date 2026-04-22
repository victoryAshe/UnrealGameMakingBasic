// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
}

void AABCharacterNonPlayer::SetDead()
{
	// 상위 로직 실행.
	Super::SetDead();

	// Timer를 사용해 일정 시간 대기 후 Actor 제거.
	// L-Value-Reference: 메모리를 차지하는 변수인 L-Value를 참조함.
	FTimerHandle TimerHandle;

	// lambda 구문
	// Capture [] : lambda 함수 본문에서 사용할 정보를 설정.
	// Parameter 선언 ()
	// Return 구문 -> // lambda는 return이 void라면 보통 생략하는 편.
	// 본문 {}
	
	//  [&]는 현재 lambda scope에서 “사용된 것을 모두 캡처”
	//	멤버 변수는 직접 캡처되지 않고 this를 통해 간접 접근
	//	현재 코드에서는 사실상 this 하나만 캡처된 상태

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this]() /* ->void */
			{
				Destroy();
			}
		),
		DeadEventDelayTime,
		false
	);
}
