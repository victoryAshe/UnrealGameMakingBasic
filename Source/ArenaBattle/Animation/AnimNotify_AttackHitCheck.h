// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UAnimNotify_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	// Animation Notify가 발생할 때 호출되는 함수.
	void Notify(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
};
