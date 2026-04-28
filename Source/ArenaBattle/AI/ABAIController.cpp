// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"

AABAIController::AABAIController()
{
	// 사용할 Blackboard Asset Load.
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(
		TEXT("/Game/ArenaBattle/AI/BB_ABCharacter.BB_ABCharacter")
	);

	if (BBAssetRef.Succeeded())
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(
		TEXT("/Game/ArenaBattle/AI/BT_ABCharacter.BT_ABCharacter")
	);

	if (BTAssetRef.Succeeded())
	{
		BTAsset = BTAssetRef.Object;
	}
}

void AABAIController::RunAI()
{
	// BlackBoard Component Pointer 가져오기. => L-Value-Reference.
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	
	// 사용할 Blackboard 지정.
	// 두 번째 Parameter가 *& type이므로 명시적 변수가 있어야 함.
	if (UseBlackboard(BBAsset, BlackboardPtr)) // Use의 성공 여부를 반환.
	{
		// Pawn의 위치를 Blackboard에 저장.
		Blackboard->SetValueAsVector(
			BBKEY_HOMEPOS,
			GetPawn()->GetActorLocation()
		);

		// BT 실행.
		bool Result = RunBehaviorTree(BTAsset);
		
		// Exception Handling.
		//ensureAlways(Result);
	}
}

void AABAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Pawn에 빙의하면 AI 로직 실행.
	RunAI();
}
