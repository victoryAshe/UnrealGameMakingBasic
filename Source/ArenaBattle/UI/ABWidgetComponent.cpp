// Fill out your copyright notice in the Description page of Project Settings.


#include "ABWidgetComponent.h"
#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();

	// Super::InitWidget()에서 로직을 따라가다보면,,
	// 함수 실행 과정에서 CreateWidget을 통해 Widget이 생성됨!
	// 그 이후에 여기가 실행됨. 따라서 Widget 초기화를 보장 받을 수 있다!

	UABUserWidget* ABUserWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUserWidget)
	{
		ABUserWidget->SetOwningActor(GetOwner());
	}
}
