// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/ABFountain.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	// Root Copmponent 지정
	RootComponent = Body;

	// Water Component를 root의 하위 Component로 지정.
	Water->SetupAttachment(Body);

	// 하위 Component 설정 후에는 Relative Location 설정해주는 게 좋다.
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 132.0f));


	// Asset 검색 후 설정.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FountainBodyMesh(
		TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01")
		);

	if (FountainBodyMesh.Succeeded())
	{
		Body->SetStaticMesh(FountainBodyMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMesh(
		TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02")
		);

	if (WaterMesh.Succeeded())
	{
		Water->SetStaticMesh(WaterMesh.Object);
	}
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

