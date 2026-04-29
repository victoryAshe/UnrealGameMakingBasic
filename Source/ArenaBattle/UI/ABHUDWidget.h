// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// UserWidget은 기본 생성자가 아닌,
	// FObjectInitializer를 Parameter로 받는 생성자를 사용.
	UABHUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	// Character에서 호출할 함수.
	// Wrapper 함수로, 이 Widget이 관리하는 하위 Widget에 Message 전달.
	void UpdateStat(
		const FABCharacterStat& BaseStat,
		const FABCharacterStat& ModifierStat
	);

	void UpdateHpBar(float NewCurrentHp);

	//HUD.
protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UABHpBarWidget> HpBar;
	
	UPROPERTY()
	TObjectPtr<class UABCharacterStatWidget> CharacterStat;
};
