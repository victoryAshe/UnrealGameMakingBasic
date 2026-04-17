// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABFountain.generated.h"

UCLASS()
class ARENABATTLE_API AABFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Component 구성
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<UStaticMeshComponent> Body;

	// 문자열로 Category를 넣으면 space를 넣을 수 있다.
	// 위와 같이 넣으면 space 넣을 수 없음.
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Water;

};
