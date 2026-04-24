// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

// 상태에 따른 처리를 위한 델리게이트.
DECLARE_DELEGATE(FOnStateChangedDelegate);

// 열거형.
// 상태를 나타내는 열거형.
UENUM(BlueprintType)
enum class EStageState : uint8
{
	Ready = 0,
	Fight,
	Reward,
	Next
};


UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AABStageGimmick();

protected:
	// 이 함수는 트랜스폼 변경 뿐 아니라, 다른 속성이 변경되더라도 호출됨.
	virtual void OnConstruction(
		const FTransform& Transform) override;


	// Stage Section.
protected:
	// 스테이지를 보여줄 메시 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Stage)
	TObjectPtr<class UStaticMeshComponent> Stage;

	// 박스 컴포넌트 (오버랩 이벤트 처리를 위한 컴포넌트).
	UPROPERTY(VisibleAnywhere, Category = Stage)
	TObjectPtr<class UBoxComponent> StageTrigger;

	// StageTrigger에 등록할 오버랩 이벤트 처리 함수.
	UFUNCTION()
	void OnStageTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// Gate Section(4개 문).
protected:
	// 4개의 문을 Map으로 관리.
	UPROPERTY(VisibleAnywhere, Category = Gate)
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;

	// 각 문을 담당하는 콜리전을 배열로 관리.
	UPROPERTY(VisibleAnywhere, Category = Gate)
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;

	// 각 문의 이벤트를 처리할 함수.
	UFUNCTION()
	void OnGateTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);


	// 문 열고 닫는 함수.
	void OpenAllGates();
	void CloseAllGates();

	// State Section.
protected:
	// 현재 상태를 나타내는 열거형 변수.
	UPROPERTY(EditAnywhere, Category = Stage)
	EStageState CurrentState;

	// 상태 설정에 사용할 함수.
	void SetState(EStageState InNewState);

	// 상태에 따른 처리를 위한 맵.
	TMap<EStageState, FOnStateChangedDelegate> StateChangedActions;

	// 각 상태 처리에 대응하는 함수.
	void SetReady();
	void SetFight();
	void SetChooseReward();
	void SetChooseNext();

	// Fight Setcion.
protected:
	// 대전할 NPC 클래스 지정.
	UPROPERTY(EditAnywhere, Category = Fight)
	TSubclassOf<class AABCharacterNonPlayer> OpponentClass;

	// NPC 생성까지 대기할 시간.
	UPROPERTY(EditAnywhere, Category = Fight)
	float OpponentSpawnTime;

	// NPC를 없앴을 때 실행할 함수.
	UFUNCTION()
	void OnOpponentDestroyed(AActor* DestroyActor);

	// 대전 상태 생성에 사용할 타이머 핸들.
	FTimerHandle OpponentSpawnTimerHandle;

	// 생성 타이머가 종료될 때 호출할 함수.
	void OnOpponentSpawn();

	// Reward Section.
protected:
	// 보상용 아이템 상자 클래스.
	UPROPERTY(VisibleAnywhere, Category = Reward)
	TSubclassOf<class AABItemBox> RewardBoxClass;

	// 생성된 아이템 박스를 저장할 배열 변수.
	// 아이템 상자는 스테이지 액터와는 무관.
	// 따라서 강참조 보다는 약참조가 적절.
	UPROPERTY(VisibleAnywhere, Category = Reward)
	TArray<TWeakObjectPtr<class AABItemBox>> RewardBoxes;

	// 아이템 상자 생성 위치 (맵으로 관리).
	TMap<FName, FVector> RewardBoxLocations;

	// 생성된 상자에서 오버랩 이벤트가 발생하면 호출할 함수.
	UFUNCTION()
	void OnRewardTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 아이템 상자 생성 함수.
	void SpawnRewardBoxes();
};
