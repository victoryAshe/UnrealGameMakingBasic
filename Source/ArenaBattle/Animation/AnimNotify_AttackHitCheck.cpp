// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(
	USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// Anim Notify가 배치된 Animation을 사용(소유)하는 Pawn에 접근해서
	// 공격 판정하도록 함수 실행.
	// Interface를 활용 -> 다른 객체에 접근해야 할 때, 알고싶은 정보만 노출.
	if (MeshComp)
	{
		IABAnimationAttackInterface* AttackPawn
			= Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());

		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}

}
