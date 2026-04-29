// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	// BaseStat, ModifierStat 정보를 받아 UI를 Update하는 함수.
	// Stat Component에 Event Binding(Delegate)해 호출되도록 설정.
	void UpdateStat(
		const FABCharacterStat& BaseStat,
		const FABCharacterStat& ModifierStat
	);

private:
	// StatWidget이 가진 TextBlock들을 저장하는 Map(for BaseStat).
	UPROPERTY()
	TMap<FName, class UTextBlock*> BaseLookup;
	
	// StatWidget이 가진 TextBlock들을 저장하는 Map(for ModifierStat).
	UPROPERTY()
	TMap<FName, class UTextBlock*> ModifierLookup;
};
