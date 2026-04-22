// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/ABCharacterWidgetInterface.h"

// 상위 객체인 UUserWidget에 기본 생성자가 없어서 아래 구문을 붙여줘야 함.
UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// 캐릭터 스탯을 전달 받아야 하므로, 
	// 초기값은 오류를 알 수 있게 음수로 설정.
	MaxHp = -1.0f;
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	// 값 검증
	ensureAlways(MaxHp > 0.0f);

	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}

void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// NativeConstruct가 호출되면, UI에 대한 초기화가 끝났다고 생각할 수 있음.
	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("Pb_HpBar")));

	// 값이 제대로 설정됐는지 확인
	ensureAlways(HpProgressBar);

	// Character(Interface)에 접근해서,
	// Delegate 등록이 가능하도록 정보 전달.
	IABCharacterWidgetInterface* CharacterWidget
		= Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}

}
