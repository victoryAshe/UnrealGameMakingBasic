// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UABComboActionData();

	// Section 접두사 (ComboAttack)
	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;

	// 최대 콤보 수(4개).
	UPROPERTY(EditAnywhere, Category = ComboData)
	uint32 MaxComboCount;

	// Frame 재생 속도 (Animation Asset에서 확인).
	UPROPERTY(EditAnywhere, Category = ComboData)
	float FrameRate;

	// 콤보 공격 마다의 콤보 판정에 사용할 Frmae 값.
	// 입력이 이 Frmae 이전에 들어왔는지 판정하는 데 활용.
	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
