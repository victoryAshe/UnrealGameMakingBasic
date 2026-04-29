// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "InputActionValue.h"
#include "Interface/ABCharacterHUDInterface.h"
#include "ABCharacterPlayer.generated.h"

// 전방선언.
class UInputAction;

/**
 *
 */
UCLASS()
class ARENABATTLE_API AABCharacterPlayer : 
	public AABCharacterBase,
	public IABCharacterHUDInterface
{
	GENERATED_BODY()

public:
	AABCharacterPlayer();

protected:
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;

	// 설정된 컨트롤에 따라 입력 매핑 컨텍스트 및 관련 설정 처리 함수.
	void SetCharacterControl(
		ECharacterControlType NewCharacterControlType
	);

	// 컨트롤 데이터 설정.
	virtual void SetCharacterContolData(
		const class UABCharacterControlData* InCharacterControlData) override;

protected:
	// 캐릭터 컨트롤 변경 입력 처리 함수.
	void ChangeCharacterControl();

	// 이동 함수.
	void ShoulderMove(const FInputActionValue& Value);

	// 회전 함수.
	void ShoulderLook(const FInputActionValue& Value);

	// 쿼터뷰일 때 이동 처리 함수.
	void QuarterMove(const FInputActionValue& Value);

	// 공격 함수.
	void Attack();

protected:
	// 컴포넌트 구성.
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;


	// 입력 관련.
protected:
	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> ShoulderMoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> ShoulderLookAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> QuarterMoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> ChangeControlAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> AttackAction;

	// 현재 사용 중인 캐릭터 컨트롤 타입.
	UPROPERTY(VisibleAnywhere, Category = CharacterControl)
	ECharacterControlType CurrentCharacterControlType;

	// Inherited via IABCharacterHUDInterface
	virtual void SetupHUDWidget(
		class UABHUDWidget* InHUDWidget) override;
};
