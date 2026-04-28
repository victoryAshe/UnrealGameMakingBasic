// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "Interface/ABCharacterAIInterface.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacterNonPlayer 
	: public AABCharacterBase,
	public IABCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AABCharacterNonPlayer();

protected:
	// 랜덤으로 메시 로드 처리를 위해 선언.
	virtual void PostInitializeComponents() override;

	// NPC 메시의 로드가 완료되면 호출될 콜백 함수.
	void NPCMeshLoadCompleted();

	// NPC에서 부가적으로 죽었을 때 처리를 위해 override.
	virtual void SetDead() override;


	// config 파일에 설정된 경로 값을 배열로 관리.
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	// 비동기 방식으로 애셋을 로드하기 위한 핸들.
	TSharedPtr<FStreamableHandle> NPCMeshHandle;

	// Inherited via IABCharacterAIInterface
	float GetAIPatrolRadius() override;
	float GetAIDetectRange() override;
	float GetAIAttackRange() override;
	float GetAITurnSpeed() override;

	virtual void AttackByAI() override;
	virtual void SetAIAttackDelegate(
		const FAICharacterAttackFinished& InOnAttackFinished
	) override;

	// SetAIAttackDelegate 함수로부터 전달받은 델리게이트를 저정할 변수.
	FAICharacterAttackFinished OnAttackFinished;

	// 공격 모션이 끝나면 호출되는 이벤트 함수.
	virtual void NotifyComboActionEnd() override;
};
