// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterStat.h"
#include "ABGameSingleton.generated.h"

/**
 *
 */
UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
public:
	UABGameSingleton();

	// 레벨을 인덱스로 사용해 스텟을 가져오는 Getter.
	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const
	{
		// 인덱스 유효성 검사.
		bool Valid = CharacterStatTable.IsValidIndex(InLevel - 1);
		return Valid ?
			CharacterStatTable[InLevel - 1] : FABCharacterStat();
	}

	// 싱글톤 Getter.
	static UABGameSingleton& Get();

	// 최대 레벨 값.
	UPROPERTY()
	int32 CharacterMaxLevel;

private:
	// 스탯을 배열로 관리.
	TArray<FABCharacterStat> CharacterStatTable;
};
