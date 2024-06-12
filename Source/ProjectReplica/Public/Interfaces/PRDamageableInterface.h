// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "UObject/Interface.h"
#include "PRDamageableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPRDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 대미지에 관련된 Interface 클래스입니다.
 */
class PROJECTREPLICA_API IPRDamageableInterface
{
	GENERATED_BODY()

public:
	/** 현재 체력을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damageable")
	float GetCurrentHealth();

	/** 최대 체력을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damageable")
	float GetMaxHealth();

	/**
	 * 입력받은 인자만큼 현재 체력을 회복하는 함수입니다.
	 *
	 * @param Amount 회복량
	 * @return 회복한 후의 체력
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damageable")
	float Heal(float Amount);

	/**
	 * 대미지를 받는 함수입니다.
	 *
	 * @param DamageInfo 대미지의 정보
	 * @return 대미지 적용 여부
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damageable")
	bool TakeDamage(FPRDamageInfo DamageInfo);
};
