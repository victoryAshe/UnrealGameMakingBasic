#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ABCharacterStat.generated.h"

// 데이터 테이블을 임포트할 때 행(Row) 데이터를 정의하는 구조체.
// Row-Data = Record(레코드).
USTRUCT(BlueprintType)
struct FABCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterStat()
		: MaxHp(0.0f),
		Attack(0.0f),
		AttackRange(0.0f),
		AttackSpeed(0.0f),
		MovementSpeed(0.0f)
	{
	}
	// 두 구조체를 더하는 연산자 오버로딩.
	FABCharacterStat operator+(const FABCharacterStat& Other)
	{
		//FMemory::Memcpy()

		// 아래 방법은 위의 memcpy랑 같은 방법이다. 
		// memcpy는 통으로 복사하기 때문에 더 낫다. 
		// 그래서 서로 다른 type이 있는 데이터도 처리 가능
		// 그래도 위험하긴 하다.


		// pointer 연산을 활용한 덧셈 처리.
		// [전제 조건]
		// 모든 속성 Data(member)의 크기가 같아야 함!
		// => 시작 주소부터 같은 크기의 byte씩 끊어서 변환함.
		// 가상 함수가 들어가면 VTable이 앞에 추가되므로, (size가 바뀜) 
		// virtual 함수가 있으면 사용할 수 없는 방법!!

		const float* const ThisPtr
			= reinterpret_cast<const float* const>(this);

		const float* const OtherPtr
			= reinterpret_cast<const float* const>(&Other);

		// 결과를 저장할 구조체.
		FABCharacterStat Result;

		// 결과 구조체 포인터. => ResultPtr에 연산을 하면 Result에 저장이 된다.
		float* ResultPtr = reinterpret_cast<float*>(&Result);

		// Loop로 처리를 위해, float 변수의 개수 확인.
		const int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);

		// Unreal에서는 for loop의 변수도 Pascal Casing을 권장.
		for (int32 Index = 0; Index < StatNum; ++Index)
		{
			// Pointer로 접근해서 배열처럼 취급.
			ResultPtr[Index] = ThisPtr[Index] + OtherPtr[Index];
		}

		return Result;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;
};