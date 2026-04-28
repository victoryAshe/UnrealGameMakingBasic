// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"

#include "Engine/OverlapResult.h"
#include "Character/ABCharacterNonPlayer.h"

#include "Item/ABItemBox.h"


// Sets default values
AABStageGimmick::AABStageGimmick()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 컴포넌트 생성.
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	// 메시 애셋 로드 및 설정.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(
		TEXT("/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE")
	);
	if (StageMeshRef.Succeeded())
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(
		TEXT("StageTrigger")
	);
	// 박스 컴포넌트 충돌 영역 크기 설정.
	StageTrigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	// 계층 설정.
	StageTrigger->SetupAttachment(Stage);
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AABStageGimmick::OnStageTriggerBeginOverlap
	);

	// 문 컴포넌트.
	static FName GateSockets[] =
	{
		TEXT("+XGate"),
		TEXT("-XGate"),
		TEXT("+YGate"),
		TEXT("-YGate"),
	};
	// 문 메시 로드.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(
		TEXT("/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE")
	);

	// 배열 순회하면서 문 메시 생성.
	for (FName GateSocket : GateSockets)
	{
		// 컴포넌트 생성.
		UStaticMeshComponent* Gate
			= CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
		// 생성된 컴포넌트에 메시 애셋 설정.
		if (GateMeshRef.Succeeded())
		{
			Gate->SetStaticMesh(GateMeshRef.Object);
		}
		// 컴포넌트 계층 설정(소켓 포함 지정).
		Gate->SetupAttachment(Stage, GateSocket);
		// 위치 / 회전 조정.
		Gate->SetRelativeLocationAndRotation(
			FVector(0.0f, -80.0f, 0.0f),
			FRotator(0.0f, -90.0f, 0.0f)
		);

		// Map에 생성한 컴포넌트 추가.
		Gates.Add(GateSocket, Gate);

		// 문 통과를 확인하기 위한 박스 컴포넌트.
		// 예: +XGate + Trigger = +XGateTrigger.
		FName TriggerName = *GateSocket.ToString().Append("Trigger");
		UBoxComponent* GateTrigger
			= CreateDefaultSubobject<UBoxComponent>(TriggerName);
		// 박스 컴포넌트의 충돌 영역 설정.
		GateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		// 생성된 박스 컴포넌트의 계층 및 소켓 설정.
		GateTrigger->SetupAttachment(Stage, GateSocket);
		// 상대위치 조정.
		GateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		// 콜리전 프로필 설정.
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
		// 충돌 이벤트에 함수 등록 (델리게이트).
		GateTrigger->OnComponentBeginOverlap.AddDynamic(
			this,
			&AABStageGimmick::OnGateTriggerBeginOverlap
		);

		// 트리거 구분을 위해 태그 추가.
		GateTrigger->ComponentTags.Add(GateSocket);

		// 생성된 트리거 컴포넌트를 배열에 추가.
		GateTriggers.Add(GateTrigger);
	}

	// 시작 상태 값 설정.
	CurrentState = EStageState::Ready;

	// 상태에 따라 처리하도록 맵 설정.
	StateChangedActions.Add(
		EStageState::Ready,
		FOnStateChangedDelegate::CreateUObject(
			this,
			&AABStageGimmick::SetReady
		)
	);

	StateChangedActions.Add(
		EStageState::Fight,
		FOnStateChangedDelegate::CreateUObject(
			this,
			&AABStageGimmick::SetFight
		)
	);

	StateChangedActions.Add(
		EStageState::Reward,
		FOnStateChangedDelegate::CreateUObject(
			this,
			&AABStageGimmick::SetChooseReward
		)
	);

	StateChangedActions.Add(
		EStageState::Next,
		FOnStateChangedDelegate::CreateUObject(
			this,
			&AABStageGimmick::SetChooseNext
		)
	);


	// Fight 관련 변수 초기값.
	OpponentSpawnTime = 2.0f;
	OpponentClass = AABCharacterNonPlayer::StaticClass();

	// 아이템 상자 생성 클래스.
	RewardBoxClass = AABItemBox::StaticClass();

	// 아이템 상자 생성 위치 설정.
	for (FName GateSocket : GateSockets)
	{
		FVector BoxLocation = Stage->GetSocketLocation(GateSocket) / 2;
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 이 때 상태 변경 처리.
	SetState(CurrentState);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 캐릭터가 스테이지에 입장하면 호출됨.
	// 이때는 대전(Fight) 상태로 변경.
	SetState(EStageState::Fight);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 부딪힌 문(Gate)의 방향을 확인해서, 
	// 적절한 소켓 위치에 새로운 스테이지 액터 생성.

// 예외 처리.
	ensureAlways(OverlappedComponent->ComponentTags.Num() == 1);

	// 태그 확인.
	FName ComponentTag = OverlappedComponent->ComponentTags[0];

	// 태그 값에서 앞의 2글자만 가져오기.
	FName SocketName = FName(*ComponentTag.ToString().Left(2));

	// 스테이지 메시에는 이미 +X, -X, +Y, -Y 소켓이 이미 지정되어 있음.
	ensureAlways(Stage->DoesSocketExist(SocketName));

	// 소켓 이름으로 위치 값 가져오기.
	FVector NewLocation = Stage->GetSocketLocation(SocketName);

	// 충돌 결과 저장용 배열 변수.
	TArray<FOverlapResult> OverlapResults;

	// 충돌 검사 진행할 때 나는 제외.
	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(GateTrigger),
		false,
		this
	);

	// 생성할 위치에 충돌 확인을 해서 이미 스테이지 있는지 검증.
	bool Result = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllStaticObjects,
		FCollisionShape::MakeSphere(775.0f),
		Params
	);

	// 무언가 이미 자리를 차지하고 있으면 생성 안함.
	if (Result)
	{
		return;
	}

	// 생성 트랜스폼.
	const FTransform SpawnTransform(
		NewLocation
	);

	// 충돌이 안됐으면(비어 있으면) 스테이지 액터 생성.
	AABStageGimmick* NewGimmick
		= GetWorld()->SpawnActorDeferred<AABStageGimmick>(
			AABStageGimmick::StaticClass(),
			SpawnTransform
		);

	if (NewGimmick)
	{
		// 새로 생성된 스테이지의 레벨 +1.
		NewGimmick->SetStageNum(CurrentStageNum + 1);

		// 생성 처리 완료.
		NewGimmick->FinishSpawning(SpawnTransform);
	}
}

void AABStageGimmick::OpenAllGates()
{
	// 문 액터 배열 순회하면서 회전 값 설정.
	for (auto Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(
			FRotator(0.0f, -90.0f, 0.0f)
		);
	}
}

void AABStageGimmick::CloseAllGates()
{
	// 문 액터 배열 순회하면서 회전 값 설정.
	for (auto Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AABStageGimmick::SetState(EStageState InNewState)
{
	// 현재 상태 업데이트.
	CurrentState = InNewState;

	// 전달된 상태가 맵에 있는지 확인 후 처리.
	if (StateChangedActions.Contains(InNewState))
	{
		StateChangedActions[InNewState].ExecuteIfBound();
	}
}

void AABStageGimmick::SetReady()
{
	// 플레이어가 입장 가능하도록 가운데 트리거 활성화.
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);

	// 플레이어가 문과 상호작용하지 않도록 문 콜리전 비활성화.
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);
	}

	// 플레이어가 들어올 수 있게 문 열기.
	OpenAllGates();
}

void AABStageGimmick::SetFight()
{
	// 가운데 트리거 비활성화.
	StageTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);

	// 문 트리거 비활성화.
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);
	}

	// 문 닫기.
	CloseAllGates();

	// 대전 상대 생성.
	// 타이머 활용.
	GetWorld()->GetTimerManager().SetTimer(
		OpponentSpawnTimerHandle,			// 타이머 핸들.
		this,								// 구독할 함수를 소유하는 객체(인스턴스).
		&AABStageGimmick::OnOpponentSpawn,	// 등록할 함수(주소).
		OpponentSpawnTime,					// 타이머 시간.
		false								// 반복여부.
	);
}

void AABStageGimmick::SetChooseReward()
{
	// 가운데 트리거 비활성화.
	StageTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);

	// 문 트리거 비활성화.
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);
	}

	// 문 닫기.
	CloseAllGates();


	// 아이템 상자 생성.
	SpawnRewardBoxes();
}

void AABStageGimmick::SetChooseNext()
{
	// 가운데 트리거 비활성화.
	StageTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);

	// 문 트리거 활성화.
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}

	// 문 열기.
	OpenAllGates();
}

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyActor)
{
	// NPC를 무찌르면 보상(Reward) 단계로 전환.
	SetState(EStageState::Reward);
}

void AABStageGimmick::OnOpponentSpawn()
{
	// 대전 상대 NPC 생성.
	// 특정 위치(원점인데 살짝 높이만 보정)를 지정해서 사용.
	const FVector SpawnLocation
		= GetActorLocation() + FVector::UpVector * 88.0f;

	// NPC 생성.
	AActor* OpponentActor = GetWorld()->SpawnActor(
		OpponentClass,
		&SpawnLocation,
		&FRotator::ZeroRotator
	);

	// 예외처리 (생성한 액터가 우리가 의도한 타입인지 확인).
	AABCharacterNonPlayer* ABOpponentCharacter
		= Cast<AABCharacterNonPlayer>(OpponentActor);

	// 형변환에 실패하면 의도한 타입이 아니기 때문에 종료.
	if (!ABOpponentCharacter)
	{
		// 선택사항..
		// 발생하면 안되는 문제.
		OpponentActor->Destroy();
		return;
	}

	// NPC가 죽었을 때 발행되는 이벤트에 함수 등록.
	OpponentActor->OnDestroyed.AddDynamic(
		this,
		&AABStageGimmick::OnOpponentDestroyed
	);
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 캐릭터가 아이템 상자를 획득하면, 나머지 상자 처리.
	for (const auto& RewardBox : RewardBoxes)
	{
		// 아이템 상자를 약참조하고 있기 때문에 확인.
		if (RewardBox.IsValid())
		{
			// 포인터 가져오기.
			AABItemBox* ValidItemBox = RewardBox.Get();

			// 부딪힌 아이템 상자 검출.
			AActor* OverlappedActor = OverlappedComponent->GetOwner();

			// 부딪히지 않은 상자는 제거.
			if (OverlappedActor != ValidItemBox)
			{
				ValidItemBox->Destroy();
			}
		}
	}

	// Next 단계로 이동.
	SetState(EStageState::Next);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	// 아이템 상자 생성.
	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		// 상자 위치.
		FVector WorldSpawnLocation
			= GetActorLocation() + RewardBoxLocation.Value
			+ FVector(0.0f, 0.0f, 30.0f);

		const FTransform SpawnTransform(
			WorldSpawnLocation
		);

		// 상자 생성.
		//AActor* ItemActor
		AABItemBox* RewardBoxActor
			= GetWorld()->SpawnActorDeferred<AABItemBox>(
				RewardBoxClass,
				SpawnTransform
			);

		// 아이템 상자로 형변환.
		//AABItemBox* RewardBoxActor
		//	= Cast<AABItemBox>(ItemActor);
		if (RewardBoxActor)
		{
			// 상자에 태그 추가.
			// 나중에 어느 상자와 부딪혔는지 알기 위해.
			RewardBoxActor->Tags.Add(RewardBoxLocation.Key);

			// 박스가 BeginOverlap될 때 발행되는 이벤트에 등록.
			RewardBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(
				this,
				&AABStageGimmick::OnRewardTriggerBeginOverlap
			);

			// 생성된 박스를 배열에 추가.
			RewardBoxes.Add(RewardBoxActor);

			// 생성 완료 처리.
			RewardBoxActor->FinishSpawning(SpawnTransform);
		}
	}
}
