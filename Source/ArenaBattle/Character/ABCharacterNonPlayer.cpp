// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"
#include "Engine/AssetManager.h"
#include "AI/ABAIController.h"
#include "CharacterStat/ABCharacterStatComponent.h"


AABCharacterNonPlayer::AABCharacterNonPlayer()
{
	// 시작할 때는 Mesh 안 보이게 설정.
	GetMesh()->SetHiddenInGame(true);

	// AIController Class 설정.
	AIControllerClass = AABAIController::StaticClass();
	
	// Map에서 Load 또는 Runtime에 Spawn(생성)되는 모든 경우.
	// 미리 지정한 AI Controller에 빙의되도록 설정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AABCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 예외처리.
	ensureAlways(NPCMeshes.Num() > 0);

	// 랜덤으로 인덱스 선택.
	int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);

	// 비동기 방식으로 애셋 로딩.
	NPCMeshHandle
		= UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			NPCMeshes[RandIndex],
			FStreamableDelegate::CreateUObject(
				this,
				&AABCharacterNonPlayer::NPCMeshLoadCompleted
			)
		);
}

void AABCharacterNonPlayer::NPCMeshLoadCompleted()
{
	// 로드가 완료되어 핸들이 유효한지 확인.
	if (NPCMeshHandle.IsValid())
	{
		// 로드된 애셋을 스켈레탈 메시로 변환.
		USkeletalMesh* NPCMesh
			= Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());

		// 메시 애셋 설정.
		if (NPCMesh)
		{
			// 스켈레탈 메시 애셋 설정.
			GetMesh()->SetSkeletalMesh(NPCMesh);

			// 메시 컴포넌트가 화면에 보이도록 설정.
			GetMesh()->SetHiddenInGame(false);
		}
	}

	// 애셋 로드에 사용했던 핸들 해제.
	NPCMeshHandle->ReleaseHandle();
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

float AABCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AABCharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float AABCharacterNonPlayer::GetAIAttackRange()
{
	// 공격 거리.
	// 캡슐 형태 = 공격 거리 + (공격 반경 x 2).
	return Stat->GetTotalStat().AttackRange
		+ (Stat->GetAttackRadius() * 2);
}

float AABCharacterNonPlayer::GetAITurnSpeed()
{
	return 2.0f;
}

void AABCharacterNonPlayer::AttackByAI()
{
	// 공격 재생.
	ProcessComboCommand();
}

void AABCharacterNonPlayer::SetAIAttackDelegate(
	const FAICharacterAttackFinished& InOnAttackFinished)
{
	// 델리게이트를 변수에 저장.
	OnAttackFinished = InOnAttackFinished;
}

void AABCharacterNonPlayer::NotifyComboActionEnd()
{
	Super::NotifyComboActionEnd();

	// 앞서 전달받은 델리게이트 실행.
	OnAttackFinished.ExecuteIfBound();
}

