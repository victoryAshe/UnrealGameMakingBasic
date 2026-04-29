// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatComponent.generated.h"

// Delegate 선언.
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /* CurrentHp */)

// Stat 정보가 변경될 때 발행할 Delegate.
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FABCharacterStat& /* BaseStat */, const FABCharacterStat& /* ModifierStat */)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	virtual void InitializeComponent() override;

public:	
	// Setter.
	void SetHp(float newHp);

	// Getter.
	//FORCEINLINE float GetMaxHp() const { return MaxHp; }

	void SetLevelStat(int32 InNewLevel);
	FORCEINLINE float GetCurrentLevel() const { return CurrentLevel; }

	FORCEINLINE void SetBaseStat(const FABCharacterStat& InBaseStat)
	{
		BaseStat = InBaseStat;
		OnStatChanged.Broadcast(BaseStat, ModifierStat);
	}

	FORCEINLINE const FABCharacterStat& GetBaseStat() const { return BaseStat; }
	FORCEINLINE const FABCharacterStat& GetModifierStat() const { return ModifierStat; }

	FORCEINLINE void SetModifierStat(
		const FABCharacterStat& InModifierStat)
	{
		ModifierStat = InModifierStat;
	}
	FORCEINLINE FABCharacterStat GetTotalStat() const
	{
		// 최종 스탯 = 기본 스탯 + 부가 스탯.
		return BaseStat + ModifierStat;
	}

	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

	FORCEINLINE float GetAttackRadius() const { return AttackRadius; }



	// Damage 적용 함수.
	float ApplyDamage(float InDamage);

public:
	// Hp를 모두 소진했을 때 발행할 Delegate.
	FOnHpZeroDelegate OnHpZero;

	// Hp가 변동될 때마다 발행할 Delegate.
	FOnHpChangedDelegate OnHpChanged;

	// Stat 변경 시 발행할 Delegate.
	FOnStatChangedDelegate OnStatChanged;

protected:
	// 체력 정보.
	
	// 최대 체력.
	// VisibleInstanceOnly: class 정보에선 안보이고, 
	// 실제로 생성된 객체 정보에서만 확인가능.
	//UPROPERTY(VisibleInstanceOnly, Category = Stat)
	//float MaxHp;

	// 현재 체력.
	// Transient(임시): 현재 체력의 경우에는 게임을 진행할 때마다 자주 변경되므로,
	// 굳이 디스크에 저장이 필요하지 않다. (게임이 끝나면 버려지는 값)
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;
		
	// 공격 반경.
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float AttackRadius;

	// 현재 레벨.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	int32 CurrentLevel;

	// 캐릭터 기본 스탯 데이터.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	FABCharacterStat BaseStat;

	// 아이템을 통해 부가로 얻을 추가 스탯 데이터.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	FABCharacterStat ModifierStat;
};
