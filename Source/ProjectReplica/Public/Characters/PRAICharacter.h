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

#pragma region PooledableInterface
protected:
	/** 오브젝트가 활성화 되었는지 확인하는 함수입니다. */
	virtual bool IsActivate_Implementation() const override;
	
	/** 오브젝트를 활성화하는 함수입니다. */
	virtual void Activate_Implementation() override;

	/** 오브젝트를 비활성화하는 함수입니다. */
	virtual void Deactivate_Implementation() override;
	
	/** 오브젝트의 PoolIndex를 반환하는 함수입니다. */
	virtual int32 GetPoolIndex_Implementation() const override;

	/** 수명을 반환하는 함수입니다. */
	virtual float GetLifespan_Implementation() const override;

	/**
	 * 수명을 설정하는 함수입니다.
	 * 
	 * @param NewLifespan 설정할 수명입니다.
	 */	
	virtual void SetLifespan_Implementation(float NewLifespan) override;
#pragma endregion

#pragma region Activate
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
