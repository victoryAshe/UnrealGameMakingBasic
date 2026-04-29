// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHUDWidget.h"
#include "ABHpBarWidget.h"
#include "ABCharacterStatWidget.h"
#include "Interface/ABCharacterHUDInterface.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct(); 

	// WidgetBlueprint에 배치된 UI Widget을 이름으로 검색.
	HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));

	// 제대로 설정됐는지 확인.
	ensureAlways(HpBar);

	CharacterStat = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));

	// 제대로 설정됐는지 확인.
	ensureAlways(CharacterStat);

	// Interface를 통해, 이 Widget을 사용할 Character에 초기화 요청.
	IABCharacterHUDInterface* HUDPawn
		= Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}

void UABHUDWidget::UpdateStat(
	const FABCharacterStat& BaseStat,
	const FABCharacterStat& ModifierStat
)
{
	// HpBar의 최대 체력 Stat update.
	FABCharacterStat TotalStat = BaseStat + ModifierStat;
	HpBar->SetMaxHp(TotalStat.MaxHp);

	// CharacterStat Widget에 Message 전달.
	CharacterStat->UpdateStat(BaseStat, ModifierStat);
}
 
void UABHUDWidget::UpdateHpBar(const float NewCurrentHp)
{
	// HpBarWidget에 Message 전달.
	HpBar->UpdateHpBar(NewCurrentHp);
}