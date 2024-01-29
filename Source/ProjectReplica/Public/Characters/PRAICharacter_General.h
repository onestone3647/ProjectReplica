// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRAICharacter.h"
#include "PRAICharacter_General.generated.h"

class UWidgetComponent;

/**
 * 일반 AI 캐릭터 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API APRAICharacter_General : public APRAICharacter
{
	GENERATED_BODY()

public:
	APRAICharacter_General();
	
#pragma region HealthBar
protected:
	/** HealthBar 위젯을 생성하는 함수입니다. */
	virtual void CreateHealthBarWidget() override;
	
protected:
	/** 캐릭터의 체력을 나타내는 WidgetComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HealthBar")
	TObjectPtr<class UWidgetComponent> HealthBarWidget;
#pragma endregion 
};
