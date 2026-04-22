// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ABCharacterBase.h"
#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABComboActionData.h"
#include "Components/CapsuleComponent.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision Profile 설정.
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);


	// 메시 컴포넌트 설정.
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// 메시 애셋 지정 (검색 필요함).
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Warrior.SK_CharM_Warrior")
	);

	// 로드 성공했으면 설정.
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// 애님 블루프린트 클래스 정보 지정.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C")
	);

	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}


	// Mesh Collision 끄기.
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// 맵 설정.
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(
		TEXT("/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder")
	);
	if (ShoulderDataRef.Succeeded())
	{
		CharacterControlManager.Add(
			ECharacterControlType::Shoulder,
			ShoulderDataRef.Object
		);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuarterDataRef(
		TEXT("/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater")
	);
	if (QuarterDataRef.Succeeded())
	{
		CharacterControlManager.Add(
			ECharacterControlType::Quarter,
			QuarterDataRef.Object
		);
	}

	// Montage 및 Action Data 기본 값 설정.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> 
		ComboAttackMontageRef(
		TEXT("/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack")
	);
	if (ComboAttackMontageRef.Succeeded())
	{
		ComboAttackMontage = ComboAttackMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UABComboActionData>
		ComboActionDataRef(
			TEXT("/Game/ArenaBattle/ComboData/ABA_ComboAction.ABA_ComboAction")
		);
	if (ComboActionDataRef.Succeeded())
	{
		ComboActionData = ComboActionDataRef.Object;
	}


	// Dead Montage 기본 값 설정.
	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		DeadMontageRef(
			TEXT("/Game/ArenaBattle/Animation/AM_Dead.AM_Dead")
		);
	if (DeadMontageRef.Succeeded())
	{
		DeadMontage = DeadMontageRef.Object;
	}
}

void AABCharacterBase::SetCharacterContolData(
	const UABCharacterControlData* InCharacterControlData)
{
	// Pawn.
	bUseControllerRotationYaw
		= InCharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement.
	GetCharacterMovement()->bUseControllerDesiredRotation
		= InCharacterControlData->bUseControllerDesiredRotation;

	GetCharacterMovement()->bOrientRotationToMovement
		= InCharacterControlData->bOrientRotationToMovement;

	GetCharacterMovement()->RotationRate
		= InCharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
	// 처음 공격 시작할 때 처리.
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	// 공격이 이미 진행 중일 때 처리.
	// 타이머 핸들의 유효 여부에 따라 분기 처리.
	if (ComboTimerHandle.IsValid())
	{
		// 입력이 제대로 들어온 경우.
		bHasNextComboCommand = true;
	}
	else
	{
		// 입력이 제대로 들어오지 않은 경우.
		bHasNextComboCommand = false;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	// 현재 콤보 단계를 1단계로 설정.
	CurrentCombo = 1;

	// 몽타주 재생.
	// 애님 인스턴스 가져오기.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 몽타주 재생 속도.
		const float AttackSpeedRate = 1.0f;

		// 몽타주 재생.
		AnimInstance->Montage_Play(ComboAttackMontage, AttackSpeedRate);

		// 몽타주 종료 이벤트에 등록할 델리게이트 설정.
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindUObject(this, &AABCharacterBase::ComboActionEnd);

		// 몽타주 재생 종료 시 발행되는 이벤트에 등록.
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, ComboAttackMontage);

		// 몽타주 재생 시 이동 안하도록 설정.
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// 콤보 타이밍을 확인하기 위한 타이머 설정.
		// 기존에 설정된 타이머를 무효화.
		ComboTimerHandle.Invalidate();

		// 타이머 설정.
		SetComboCheckTimer();
	}
}

void AABCharacterBase::ComboActionEnd(
	UAnimMontage* TargetMontage, bool bInterrupted)
{
	// 값 확인 (어서트).
	ensureAlways(CurrentCombo > 0);

	// 콤보 단계 초기화.
	CurrentCombo = 0;

	// 몽타주 재생이 종료되면 캐릭터 이동을 다시 원상 복구.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
	// 현재 재생중인 콤보 단계의 인덱스 계산.
	const int32 ComboIndex = CurrentCombo - 1;

	// 확인.
	ensureAlways(
		ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex)
	);

	// 애니메이션 재생 속도도 고려(배속).
	const float AttackSpeedRate = 1.0f;

	// 타이머 설정에 사용할 시간 값.
	// 콤보 액션 데이터에는 프레임 값이 설정되어 있음.
	// 프레임(애니메이션) -> 초단위로 변환이 필요함.
	// 대략 17/30 -> 0.56666초.
	float ComboEffectTime = (ComboActionData->EffectiveFrameCount[ComboIndex]
		/ ComboActionData->FrameRate) / AttackSpeedRate;

	// 타이머 설정.
	if (ComboEffectTime > 0)
	{
		// 시간은 월드가 관리.
		GetWorld()->GetTimerManager().SetTimer(
			ComboTimerHandle,
			this,
			&AABCharacterBase::ComboCheck,
			ComboEffectTime,
			false
		);
	}
}

void AABCharacterBase::ComboCheck()
{
	// 타이머 재사용을 위해 타이머 핸들 초기화.
	ComboTimerHandle.Invalidate();

	// 콤보 타이머 전에 공격 입력이 들어왔는지 확인.
	if (bHasNextComboCommand)
	{
		// 몽타주 섹션 점프.
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 다음 콤보 단계 설정.
			CurrentCombo = FMath::Clamp(
				CurrentCombo + 1,
				1,
				ComboActionData->MaxComboCount
			);

			// 점프할 섹션 이름 구성.
			FName NextSection = *FString::Printf(
				TEXT("%s%d"),
				*ComboActionData->MontageSectionNamePrefix,
				CurrentCombo
			);

			// 몽타주 섹션 점프.
			AnimInstance->Montage_JumpToSection(
				NextSection,
				ComboAttackMontage
			);

			// 타이머 재설정.
			SetComboCheckTimer();

			// 콤보 처리에 사용한 값 초기화.
			bHasNextComboCommand = false;
		}
	}
}


void AABCharacterBase::AttackHitCheck()
{
	// 공격 범위 상수.
	const float AttackRange = 100.0f;

	// Trace에 사용할 구체의 반지름
	const float AttackRadius = 30.0f;

	// Collision에 사용할 Collision Parameter 설정.
	// 스스로를 제외해달라고 설정하기 위함!
	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(Attack),	// 이 MACRO에 Name을 넘기면 알아서 Hash값과 Query를 생성.
		false,						// 복잡하게 처리할 것인지 여부
		this						// 제외할 Actor
	);

	// 충돌 판정 결과 정보를 반환 받을 함수.
	FHitResult OutHitResult;

	// Trace 시작 위치.
	// Actor 위치 + Capsule의 반지름 만큼 앞으로 떨어뜨린 위치.
	FVector Start
		= GetActorLocation()
		+ GetActorForwardVector()								// 앞 방향 (1cm)
		* GetCapsuleComponent()->GetScaledCapsuleRadius();		// 반지름

	// Trace 끝 위치.
	// 시작 위치 + 공격 범위 앞으로 떨어뜨린 위치.
	FVector End
		= Start
		+ GetActorForwardVector()
		* AttackRange;

	// 충돌 판정 처리.
	bool HitDetected = 
		GetWorld()->SweepSingleByChannel(
		OutHitResult,								// 반환해줄 Result.
		Start,										// 시작 위치
		End,										// 끝 위치
		FQuat::Identity,
		CCHANNEL_ABACTION,							// 검출할 TraceChannel
		FCollisionShape::MakeSphere(AttackRadius),	// 검사할 Sphere
		Params										// 어떻게 검사할지 parameter.
	);

	// 충돌이 감지되었으면 데미지 전달.
	if (HitDetected)
	{
		// 전달할 데미지 양.
		const float Damage = 30.0f;
		
		// Damage Event 변수.
		FDamageEvent DamageEvent;

		// 충돌이 감지된 Actor에 TakeDamage 함수 호출.
		OutHitResult.GetActor()->TakeDamage(
			Damage,
			DamageEvent,
			GetController(),
			this
			);
	}

	// 디버그 구성일 때만 그리도록 전처리.
#if ENABLE_DRAW_DEBUG

	// Capsule의 중심 위치.
	FVector CapsuleOrigin = Start + (End - Start) * 0.5;

	// Capsule 높이의 반.
	float CapsuleHalfHeight = AttackRange * 0.5f;

	// 표시할 색상
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	// 충돌 디버깅 정보 시각화.
	DrawDebugCapsule(
		GetWorld(),				// 어떤 월드인지
		CapsuleOrigin,			// 그릴 Capsule의 중심 위치
		CapsuleHalfHeight,		// 그릴 Capsule의 높이.
		AttackRadius,			// 그릴 Capsule의 반지름.
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), // 옆으로 뒤집기
									// 앞방향으로 Z축을 새로 만들어주고, Quaternion으로 변환.
		DrawColor,				// 그릴 색상
		false,					// 지속할 것인지
		5.0f					// 몇 초 동안 그릴 것인지.
	);

#endif

}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser
	);

	// 죽음 설정. (한 번만 맞도록 죽도록)
	SetDead();

	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	// 죽었을 때 필요한 정리 작업.


	// 무브먼트 끄기.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 죽는 모션 재생. (Montage 재생 요청).
	PlayDeadAnimation();

	// Collision 끄기.
	SetActorEnableCollision(false);
}

void AABCharacterBase::PlayDeadAnimation()
{
	// Montage 재생을 위해 Anim Instance 가져오기.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 재생 중일 수 있는 Montage 모두 종료.
		AnimInstance->StopAllMontages(0.0f); // Blend 시간 값.

		// 죽음 Montage 재생.
		AnimInstance->Montage_Play(DeadMontage);
	}
}
