// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABUserWidget.h"
#include "ABHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHpBarWidget : public UABUserWidget
{
	GENERATED_BODY()
	
public:
	// UserWidget은 기본 생성자가 아닌,
	// FObjectInitializer를 Parameter로 받는 생성자를 사용.
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);

	// Setter.
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }

	// 체력 변경 Event (Delegate)에 등록할 함수.
	void UpdateHpBar(float NewCurrentHp);

protected:

	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

protected:
	// Hp 게이지를 보여주기 위해 사용할 ProgressBar.
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	// 최대 체력 값 설정 (게이지 계산을 위해).
	UPROPERTY()
	float MaxHp;
};
