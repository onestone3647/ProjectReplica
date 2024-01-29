// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PRDamageAmountWidget.h"
#include "ProjectReplicaGameInstance.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

UPRDamageAmountWidget::UPRDamageAmountWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DamageAmountTextBlock = nullptr;
	CriticalImageCanvasPanel = nullptr;
	FadeOutWidgetAnim = nullptr;
}

void UPRDamageAmountWidget::InitializeDamageAmountWidget(float DamageAmount, bool bIsCritical, EPRElement Element)
{
	if(FadeOutWidgetAnim)
	{
		BindToAnimationFinished(FadeOutWidgetAnim, OnFadeOutWidgetAnimFinishedDelegate);
		PlayAnimationForward(FadeOutWidgetAnim);
	}
	
	if(DamageAmountTextBlock)
	{
		// Text 변경
		int32 TruncatedDamage = FMath::TruncToFloat(DamageAmount);	// DamageAmount를 정수로 변환하여 소수점 아래를 버립니다.
		FString FloatAsString = FString::Printf(TEXT("%d"), TruncatedDamage);
		FText TextValue = FText::FromString(FloatAsString);
		DamageAmountTextBlock->SetText(TextValue);
	}

	// 치명타 이미지 설정
	if(CriticalImageCanvasPanel)
	{
		if(bIsCritical)
		{
			CriticalImageCanvasPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			CriticalImageCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// 색상 변경
	UProjectReplicaGameInstance* PRGameInstance = Cast<UProjectReplicaGameInstance>(GetGameInstance());
	if(PRGameInstance)
	{
		SetColorAndOpacity(PRGameInstance->GetElementColor(Element));
	}
}
