// Fill out your copyright notice in the Description page of Project Settings.


#include "ABItemBox.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/ABCollision.h"

#include "Interface/ABCharacterItemInterface.h"
#include "ABItemData.h"

// Sets default values
AABItemBox::AABItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	// 컴포넌트 생성.
	Trigger = CreateDefaultSubobject<UBoxComponent>(
		TEXT("TriggerBox")
	);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("Mesh")
	);
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(
		TEXT("Effect")
	);

	// 루트 컴포넌트 설정.
	RootComponent = Trigger;

	// 계층 설정.
	Mesh->SetupAttachment(Trigger);
	Effect->SetupAttachment(Trigger);

	// 콜리전 프로필 설정.
	Trigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);

	// 박스 크기 조정.
	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));

	// 델리게이트에 함수 등록.
	Trigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AABItemBox::OnOverlapBegin
	);

	// 애셋 로드 (메시).
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(
		TEXT("/Game/ArenaBattle/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1")
	);
	if (BoxMeshRef.Succeeded())
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}

	// 메시 컴포넌트 설정.
	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	// 메시 컴포넌트는 충돌하지 않도록 설정.
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	// 애셋 로드 (파티클 애셋).
	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(
		TEXT("/Game/ArenaBattle/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh")
	);
	if (EffectRef.Succeeded())
	{
		Effect->SetTemplate(EffectRef.Object);
		// 바로 재생되지 않도록 설정.
		Effect->bAutoActivate = false;
	}
}

void AABItemBox::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult
)
{
	// 꽝도 있다고 가정.
	if (!Item)
	{
		Destroy();
		return;
	}

	// Item이 설정되어있으면, 
	// 캐릭터에 아이템 획득 메시지 전달 (Interface를 통해).
	IABCharacterItemInterface* OverlappingPawn
		= Cast<IABCharacterItemInterface>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->TakeItem(Item);
	}

	// 파티클 재생.
	Effect->Activate();

	// 메시는 비활성화.
	Mesh->SetHiddenInGame(true);

	// 콜리전 끄기.
	SetActorEnableCollision(false);

	// 파티클 재생이 종료되면 액터를 제거하기 위해 델리게이트 등록.
	Effect->OnSystemFinished.AddDynamic(
		this,
		&AABItemBox::OnEffectFinished
	);
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}
