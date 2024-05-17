// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Objects/PRDamageableObject.h"
#include "PRDamageableObject_HasHealthPoint.generated.h"

class UWidgetComponent;

/**
 * HealthPoint를 가지고 있어, 일정 횟수 이상 피격 당하거나 일정 대미지 이상 피격 당할 경우 파괴되는 오브젝트 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRDamageableObject_HasHealthPoint : public APRDamageableObject
{
	GENERATED_BODY()

public:
	APRDamageableObject_HasHealthPoint();

protected:
	/** 액터에 속한 모든 컴포넌트의 세팅이 완료되면 호출되는 함수입니다. */
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

protected:
	/** 오브젝트를 초기화하는 함수입니다. */
	void InitializeObject();

protected:
	/** 오브젝트의 현재 체력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDamageableObject")
	float MaxHealth;

	/** 오브젝트의 현재 체력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRDamageableObject")
	float Health;

#pragma region Interface_Damageable
public:
	/** 현재 체력을 반환하는 함수입니다. */
	virtual float GetCurrentHealth_Implementation() override;

	/** 최대 체력을 반환하는 함수입니다. */
	virtual float GetMaxHealth_Implementation() override;

	/**
	 * 대미지를 받는 함수입니다.
	 *
	 * @param DamageInfo 대미지의 정보
	 * @return 대미지 적용 여부
	 */
	virtual bool TakeDamage_Implementation(FPRDamageInfo DamageInfo) override;
#pragma endregion
	
#pragma region HealthBar
protected:
	/** HealthBar 위젯을 생성하는 함수입니다. */
	virtual void CreateHealthBarWidget();
	
protected:
	/** 오브젝트의 체력을 나타내는 WidgetComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HealthBar")
	TObjectPtr<class UWidgetComponent> HealthBarWidget;
	
	/** HealthBar 위젯의 클래스 레퍼런스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthBar")
	TSubclassOf<class UUserWidget> HealthBarWidgetClass;
#pragma endregion 
};
