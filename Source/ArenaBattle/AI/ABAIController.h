// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()

public:
	AABAIController();

	// AI 실행 함수.
	void RunAI();

	// AI 중단 함수.
	void StopAI();

protected:
	// Controller가 Pawn에 빙의했을 때 호출되는 함수.
	// BT/BB Asset 설정 및 실행 처리.
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	// Blackboard Asset.
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	// BT Asset.
	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;
};
