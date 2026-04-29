// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABCharacterStatWidget.h"
#include "Components/TextBlock.h"

void UABCharacterStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Map 구성을 위해 Reflection 활용.
	// FABCharacterStat 구조체 정보에서 변수 이름을 활용.
	for (TFieldIterator<FNumericProperty> PropIt(
		FABCharacterStat::StaticStruct());
		PropIt != nullptr;
		++PropIt)
	{

		// 속성의 이름 값.
		const FName PropKey(PropIt->GetName());

		// 속성의 이름 값을 활용해 TextBlock의 이름 값 만들기.
		// Base Stat에 대한 이름 값.
		const FName TextBaseControlName
			= *FString::Printf(
				TEXT("Txt%sBase"), *PropIt->GetName()
			);

		// Modifier Stat에 대한 이름 값.
		const FName TextModifierControlName
			= *FString::Printf(
				TEXT("Txt%sModifier"), *PropIt->GetName()
			);

		// Widget Blueprint에서 Widget Control 읽어오기.
		UTextBlock* BaseTextBlock
			= Cast<UTextBlock>(GetWidgetFromName(TextBaseControlName));

		if (BaseTextBlock)
		{
			// Map에 추가.
			BaseLookup.Add(PropKey, BaseTextBlock);
		}

		UTextBlock* ModifierTextBlock
			= Cast<UTextBlock>(GetWidgetFromName(TextModifierControlName));

		if (ModifierTextBlock)
		{
			// Map에 추가.
			ModifierLookup.Add(PropKey, ModifierTextBlock);
		}
	}
}

void UABCharacterStatWidget::UpdateStat(
	const FABCharacterStat& BaseStat,
	const FABCharacterStat& ModifierStat)
{
	for (TFieldIterator<FNumericProperty> PropIt(
		FABCharacterStat::StaticStruct());
		PropIt != nullptr;
		++PropIt)
	{
		// 속성 키 값.
		const FName PropKey(PropIt->GetName());

		// Reflection을 활용해 변수의 값 읽어오기.
		float BaseData = 0.0f;
		PropIt->GetValue_InContainer(
			reinterpret_cast<const void*>(&BaseStat), &BaseData
		);

		float ModifierData = 0.0f;
		PropIt->GetValue_InContainer(
			reinterpret_cast<const void*>(&ModifierStat), &ModifierData
		);

		// Map에서 Base Text Block을 가져와 값 설정.
		// Find는 Template 코드고, 원래 parameter의 값을 찾으면 값의 pointer를 반환
		// 지금 우리가 찾는 PropKey가 TextBlock*이기 때문에 2차 포인터가 됨.
		UTextBlock** BaseTextBlockPtr = BaseLookup.Find(PropKey);
		if (BaseTextBlockPtr)
		{
			// Float값을 FString으로 변환.
			FString FloatValue = FString::SanitizeFloat(BaseData);

			// FString을 FText로 변환해서 TextBlock에 설정.
			(*BaseTextBlockPtr)->SetText(FText::FromString(FloatValue));
		}


		UTextBlock** ModifierTextBlockPtr = ModifierLookup.Find(PropKey);
		if (ModifierTextBlockPtr)
		{
			// Float값을 FString으로 변환.
			FString FloatValue = FString::SanitizeFloat(ModifierData);

			// FString을 FText로 변환해서 TextBlock에 설정.
			(*ModifierTextBlockPtr)->SetText(FText::FromString(FloatValue));
		}
	}
	
}
