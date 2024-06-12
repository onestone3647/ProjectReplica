// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Characters/PRBaseCharacter.h"
#include "Interfaces/PRPoolableInterface.h"
#include "PRAICharacter.generated.h"

/**
 * AI 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRAICharacter : public APRBaseCharacter, public IPRPoolableInterface
{
	GENERATED_BODY()

public:
	APRAICharacter();

protected:
	virtual void BeginPlay() override;

#pragma region Activate
public:
	/** AI 캐릭터가 활성화되었는지 판별하는 함수입니다. */
	virtual bool IsActivate_Implementation() const override;	
	
	/** AI 캐릭터를 활성화하는 함수입니다. */
	virtual void Activate_Implementation() override;

	/** AI 캐릭터를 비활성화 하는 함수입니다. */
	virtual void Deactivate_Implementation() override;

protected:
	/** AI 캐릭터의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Activate")
	bool bActivate;
#pragma endregion 

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

#pragma region Attack
protected:
	/** 공격을 실행하는 함수입니다. */
	virtual void Attack_Implementation();
#pragma endregion 
};
