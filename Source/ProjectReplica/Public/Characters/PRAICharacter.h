// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "PRAICharacter.generated.h"

/**
 * AI 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRAICharacter : public APRBaseCharacter
{
	GENERATED_BODY()

public:
	APRAICharacter();

protected:
	virtual void BeginPlay() override;

#pragma region HealthBar
protected:
	/**
	 * HealthBar 위젯을 생성하는 함수입니다.
	 * 자식 클래스에서 Override해서 사용합니다.
	 */
	virtual void CreateHealthBarWidget();
	
protected:
	/** HealthBar 위젯의 클래스 레퍼런스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthBar")
	TSubclassOf<class UUserWidget> HealthBarWidgetClass;
#pragma endregion 
};
