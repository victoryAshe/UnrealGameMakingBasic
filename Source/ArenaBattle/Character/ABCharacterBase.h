// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "Interface/ABCharacterItemInterface.h"
#include "ABCharacterBase.generated.h"

// 열거형 (입력 컨트롤을 관리하기 위함).
UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quarter
};

// 아이템 획득 처리를 위한 Delegate 선언.
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UABItemData* /* InItemData */)

// 다중 상속: 여러 부모를 상속하는 형태.
// -> 문제를 일으킬 가능성이 있으므로, 요즘은 사용이 비권장.
// -> "클래스 상속"은 하나만 허용
// -> "나머지 다중 상속은 모두 Interface(순소 가상함수만 갖는 class)만"
// -> 순수 가상함수를 가진 class는 그 자체로는 instance(개체)를 생성할 수 없음.
// -> 함수 본문이 없기 때문! 자식 개체가 받아서 구현했을 때만 개체 생성 가능.
// -> 함수 본문이 없으면 왜 안될까? 
//    => 선언만 있고 본문이 없기 때문에, 링커가 연결을 못해서 링커 오류가 남.

UCLASS()
class ARENABATTLE_API AABCharacterBase 
	: public ACharacter,
	public IABAnimationAttackInterface,
	public IABCharacterWidgetInterface,
	public IABCharacterItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

	// 공격 감지 함수.
	virtual void AttackHitCheck() override;

	// Damage 처리 함수.
	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;

	// Inherited via IABCharacterWidgetInterface
	virtual void SetupCharacterWidget(UABUserWidget* InUserWidget) override;

	// 모든 Component의 초기화가 끝나면 실행됨.
	virtual void PostInitializeComponents() override;

	// Dead.
protected:
	// 죽음 설정 함수.
	virtual void SetDead();

	// 죽는 Animation 재생 함수.
	void PlayDeadAnimation();

protected:

	// 컨트롤 데이터 설정.
	virtual void SetCharacterContolData(
		const class UABCharacterControlData* InCharacterControlData
	);

	// 콤보 공격 처리 함수.
	// 공격을 처음 시작할 때와 콤보 액션을 진행할 때 실행.
	void ProcessComboCommand();

	// 콤보 공격이 시작될 때 실행할 함수.
	void ComboActionBegin();

	// 몽타주 재생 종료 시 호출할 함수 (델리게이트와 연동).
	void ComboActionEnd(UAnimMontage* TargetMontage, bool bInterrupted);
	
	// NPC가 공격이 끝나는 지점을 알 수 있도록 함수 선언.
	virtual void NotifyComboActionEnd();

	// 콤보 타이머 설정 함수.
	void SetComboCheckTimer();

	// 콤보 타이밍 처리 함수.
	// 타이머를 설정하고, 설정한 시간을 지나면 실행.
	// 시간 전에 공격 입력이 제대로 들어왔는지 확인.
	void ComboCheck();

	// 아이템 습득 시 호출될 함수 (Interface를 통해).
	virtual void TakeItem(UABItemData* InItemData) override;

	// Item 종류마다 처리될 함수.
	virtual void DrinkPotion(UABItemData* InItemData);
	virtual void EquipWeapon(UABItemData* InItemData);
	virtual void ReadScroll(UABItemData* InItemData);

	// Item 처리를 위한 Delegate 관리 배열.
	TArray<FOnTakeItemDelegate> TakeItemActions;


protected:
	// 컨트롤 타입 별 데이터 관리를 위한 맵.
	UPROPERTY(EditAnywhere, Category = CharacterControl)
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

protected:
	// 콤보 공격 몽타주.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UAnimMontage> ComboAttackMontage;

	// 콤보 액션 처리 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	TObjectPtr<class UABComboActionData> ComboActionData;

	// 현재 콤보 단계 (0은 실행 안된 상태. 1/2/3/4로 단계 구분).
	UPROPERTY(EditAnywhere, Category = Attack)
	uint32 CurrentCombo = 0;

	// 콤보 판정에 사용할 타이머.
	FTimerHandle ComboTimerHandle;

	// 콤보 점프를 판정할 때 사용할 플래그.
	UPROPERTY(VisibleAnywhere, Category = Attack)
	bool bHasNextComboCommand = false;



	// Dead 관련.
protected:
	// 죽음 Montage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dead)
	TObjectPtr<class UAnimMontage> DeadMontage;

	// 죽은 후 대기할 시간 값. (단위: 초).
	float DeadEventDelayTime = 5.0f;

protected:
	// Stat Component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	TObjectPtr<class UABCharacterStatComponent> Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	TObjectPtr<class UABWidgetComponent> HpBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	public:
		int32 GetLevel() const;
		void SetLevel(int32 InNewLevel);
};
