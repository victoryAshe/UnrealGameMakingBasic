// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();

protected:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 오너를 저장해두고 재활용.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<class UCharacterMovementComponent> Movement;

	// Idle - Move 전환을 위해 사용할 속도.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	FVector Velocity;

	// 이동 빠르기 (BlendSpace에 적용.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float GroundSpeed;

	// 이동하는지 멈춰있는지 확인하는 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsIdle : 1;

	// 이동 여부를 판단할 때 사용할 문턱 값.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float MovingThreshold;

	// 점프 확인용 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsJumping : 1;

	// 점프 여부를 판단할 때 사용할 문턱 값.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float JumpingThreshold;
};
