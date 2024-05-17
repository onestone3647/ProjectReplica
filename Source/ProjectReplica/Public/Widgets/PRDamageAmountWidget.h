// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Blueprint/UserWidget.h"
#include "PRDamageAmountWidget.generated.h"

class UTextBlock;
class UCanvasPanel;

/**
 * 대미지를 표시하는 UserWidget 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRDamageAmountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPRDamageAmountWidget(const FObjectInitializer& ObjectInitializer);

public:
	/**
	 * 위젯을 초기화하는 함수입니다.
	 *
	 * @param DamageAmount 대미지의 양
	 * @param bIsCritical 일반 대미지인지, 치명타 대미지인지 판별하는 인자
	 * @param ElementType 대미지의 속성
	 */
	void InitializeDamageAmountWidget(float DamageAmount, bool bIsCritical, EPRElementType ElementType);

private:
	/** 대미지를 나타내는 TextBlock입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "DamageAmount", meta = (AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<class UTextBlock> DamageAmountTextBlock;

	/** 치명타 이미지를 나타는 CanvasPanel입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "DamageAmount", meta = (AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<class UCanvasPanel> CriticalImageCanvasPanel;	
	
	/** 위젯이 FadeOut하는 WidgetAnimation입니다. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "DamageAmount|WidgetAnimation", meta = (AllowPrivateAccess = "true", BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutWidgetAnim;

public:
	/** FadeOutWidgetAnim이 끝났을 때 실행하는 Delegate입니다. */
	FWidgetAnimationDynamicEvent OnFadeOutWidgetAnimFinishedDelegate;
};
