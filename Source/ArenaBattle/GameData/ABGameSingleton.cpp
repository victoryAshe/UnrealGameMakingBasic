// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameSingleton.h"

UABGameSingleton::UABGameSingleton()
{
	// 데이터 테이블 애셋 로드.
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(
		TEXT("/Game/ArenaBattle/GameData/ABCharacterStatTable.ABCharacterStatTable")
	);

	// 애셋 로드 성공 시 설정.
	if (DataTableRef.Succeeded())
	{
		// UDataTable 변수 저장.
		const UDataTable* DataTable = DataTableRef.Object;

		// 유효성 검사.
		ensureAlways(DataTable->GetRowMap().Num() > 0);

		// Value 값만 배열에 저장하기 위한 배열.
		TArray<uint8*> ValueArray;

		// Value 값만 배열에 저장.
		DataTable->GetRowMap().GenerateValueArray(ValueArray);

		// 참고: forloop.
		//for (uint8* Value : ValueArray)
		//{
		//	CharacterStatTable.Add(
		//		*reinterpret_cast<FABCharacterStat*>(Value)
		//	);
		//}

		// 알고리즘을 활용해 원하는 타입으로 변환.
		Algo::Transform(
			ValueArray,
			CharacterStatTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FABCharacterStat*>(Value);
			}
		);

		// 스탯 테이블의 개수를 최대 레벨로 설정.
		CharacterMaxLevel = CharacterStatTable.Num();

		// 문제 없는지 확인.
		ensureAlways(CharacterMaxLevel > 0);
	}
}

UABGameSingleton& UABGameSingleton::Get()
{
	// 프로젝트 세팅에서 싱글톤으로 지정하면,
	// 엔진 실행 시 GameSingleton 변수에 생성해줌.
	UABGameSingleton* Singleton
		= CastChecked<UABGameSingleton>(GEngine->GameSingleton);

	//return *Singleton;

	if (Singleton)
	{
		return *Singleton;
	}

	// 오류 상황.
	UE_LOG(LogTemp, Error, TEXT("Invalid Game Singleton"));

	// 함수 구현을 위해 반환이 필요함.
	return *NewObject<UABGameSingleton>();
}