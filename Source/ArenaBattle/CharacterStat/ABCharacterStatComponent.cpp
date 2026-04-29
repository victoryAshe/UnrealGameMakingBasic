// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"


// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 현재 레벨 설정.
	CurrentLevel = 1;

	// 공격 반경 설정.
	AttackRadius = 50.0f;

	bWantsInitializeComponent = true;
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 시작할 때 현재 레벨에 맞는 스탯 데이터 설정.
	SetLevelStat(CurrentLevel);
	SetHp(BaseStat.MaxHp);
}


float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	// Damage 처리.
	// 변경 여부 확인 위해 Damage 처리 전 값 저장.
	const float PrevHp = CurrentHp;

	// Damage 값 가져오기.
	const float ActualDamage
		= FMath::Clamp<float>(InDamage, 0.0f, GetTotalStat().MaxHp);

	// Damage 값 업데이트.
	SetHp(PrevHp - ActualDamage);

	// 죽었는지 여부 (체력이 0인지) 확인.
	// 오차 범위.
	if (CurrentHp <= UE_KINDA_SMALL_NUMBER)
	{
		// 체력을 모두 소진했다는 Event 발행 => 죽음
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}


void UABCharacterStatComponent::SetHp(float NewHp)
{
	// 현재 체력 값 갱신.
	CurrentHp = FMath::Clamp<float>
	(
		NewHp,
		0.0f,
		BaseStat.MaxHp
	);

	// 체력 변경 Event 발행.
	OnHpChanged.Broadcast(CurrentHp);
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	// 게임 싱글톤의 값을 사용해 현재 레벨 고정.
	// 새로 설정할 레벨의 값이 최대 레벨을 벗어나지 않도록.
	CurrentLevel = FMath::Clamp(
		InNewLevel,
		1,
		UABGameSingleton::Get().CharacterMaxLevel
	);

	// 스탯 데이터 설정.
	//BaseStat = UABGameSingleton::Get().GetCharacterStat(CurrentLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(CurrentLevel));

	// 확인.
	ensureAlways(BaseStat.MaxHp > 0.0f);
}

