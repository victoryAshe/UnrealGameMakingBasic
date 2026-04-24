// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

	// Getter.
	FORCEINLINE class UBoxComponent* GetTrigger() { return Trigger; }

protected:
	// 컴포넌트 초기화가 끝나면 호출되는 이벤트 함수.
	virtual void PostInitializeComponents() override;

protected:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 파티클 재생 종료 시 발행되는 이벤트에 구독할 함수.
	UFUNCTION()
	void OnEffectFinished(
		class UParticleSystemComponent* PSystem);



protected:
	// 박스 컴포넌트(충돌).
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	// 상자를 보여주기 위한 메시 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	// 파티클 시스템 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UParticleSystemComponent> Effect;


	// 아이템 정보.
	UPROPERTY(EditAnywhere, Category = Item)
	TObjectPtr<class UABItemData> Item;
};
