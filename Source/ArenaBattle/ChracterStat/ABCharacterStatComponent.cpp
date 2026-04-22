// Fill out your copyright notice in the Description page of Project Settings.


#include "ChracterStat/ABCharacterStatComponent.h"
#include "ABCharacterStatComponent.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 게임이 시작되면 최대 체력에서 시작하도록 설정.
	MaxHp = 200.0f;
	CurrentHp = MaxHp;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작되면 최대 체력에서 시작하도록 설정.
	SetHp(MaxHp);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	// Damage 처리.
	// 변경 여부 확인 위해 Damage 처리 전 값 저장.
	const float PrevHp = CurrentHp;

	// Damage 값 가져오기.
	const float ActualDamage
		= FMath::Clamp<float>(InDamage, 0.0f, MaxHp);

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
		MaxHp
	);

	// 체력 변경 Event 발행.
	OnHpChanged.Broadcast(CurrentHp);
}

