// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PRBaseActorComponent.h"
#include "Engine/DataTable.h"
#include "PRStatSystemComponent.generated.h"

/**
 * 캐릭터의 능력치를 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRCharacterStat()
		: MaxHealth(300.0f)
		, Health(300.0f)
		, AttackPoint(80.0f)
		, DefencePoint(60.0f)
		, CriticalRate(15.0f)
		, CriticalDamage(150.0f)
	{}

	FPRCharacterStat(float NewMaxHealth, float NewHealth, float NewAttackPoint, float NewDefencePoint, float NewCriticalRate,
						float NewCriticalDamage)
		: MaxHealth(NewMaxHealth)
		, Health(NewHealth)
		, AttackPoint(NewAttackPoint)
		, DefencePoint(NewDefencePoint)
		, CriticalRate(NewCriticalRate)
		, CriticalDamage(NewCriticalDamage)
	{}

public:
	/** 최대 체력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat")
	float MaxHealth;

	/** 현재 체력입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStat", Transient)
	float Health;

	// /**
	//  * 캐릭터의 최종 공격력입니니다.
	//  * 캐릭터의 공격력과 장비, 스킬 버프로 증가된 공격력을 합한 값입니다.
	//  */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStat")
	// float FinalAttackPoint;

	/** 캐릭터의 공격력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat")
	float AttackPoint;

	// /**
	//  * 캐릭터의 최종 방어력입니니다.
	//  * 캐릭터의 공격력과 장비, 스킬 버프로 증가된 방어력을 합한 값입니다.
	//  */
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStat")
	// float FinalDefencePoint;

	/** 캐릭터의 방어력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat")
	float DefencePoint;

	/** 캐릭터의 치명타 확률입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float CriticalRate;

	/** 캐릭터의 치명타 피해량입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat")
	float CriticalDamage;

	// /** 궁극기 회복효율 */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	// float RegenerationRate;

	/** 속성별 피해 보너스 */

public:
	/**
	 * 인자로 받은 캐릭터의 능력치와 같은지 판별하는 == 연산자 오버로딩입니다.
	 * 
	 * @param NewPRCharacterStat 비교하는 캐릭터의 능력치입니다.
	 * @return 인자로 받은 캐릭터의 능력치와 같을 경우 true를 다를 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRCharacterStat& NewPRCharacterStat) const
	{
		return this->MaxHealth == NewPRCharacterStat.MaxHealth
				&& this->AttackPoint == NewPRCharacterStat.AttackPoint
				&& this->DefencePoint == NewPRCharacterStat.DefencePoint
				&& this->CriticalRate == NewPRCharacterStat.CriticalRate
				&& this->CriticalDamage == NewPRCharacterStat.CriticalDamage;
	}

	/**
	 * 인자로 받은 캐릭터의 능력치와 다른지 판별하는 != 연산자 오버로딩입니다.
	 * 
	 * @param NewPRCharacterStat 비교하는 캐릭터의 능력치입니다.
	 * @return 인자로 받은 캐릭터의 능력치와 다를 경우 true를 같을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRCharacterStat& NewPRCharacterStat) const
	{
		return this->MaxHealth != NewPRCharacterStat.MaxHealth
				|| this->AttackPoint != NewPRCharacterStat.AttackPoint
				|| this->DefencePoint != NewPRCharacterStat.DefencePoint
				|| this->CriticalRate != NewPRCharacterStat.CriticalRate
				|| this->CriticalDamage != NewPRCharacterStat.CriticalDamage;
	}
};

/**
 * 캐릭터의 능력치를 관리하는 ActorComponent 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRStatSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRStatSystemComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "StatSystem")
	void SetHealth(float NewHealth);
	
protected:
	/** 캐릭터의 능력치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatSystem")
	FPRCharacterStat CharacterStat;

public:
	/** CharacterStat을 반환하는 함수입니다. */
	FPRCharacterStat GetCharacterStat() const { return CharacterStat; };

	/** 입력받은 인자로 CharacterStat을 설정하는 함수입니다. */
	void SetCharacterStat(FPRCharacterStat NewCharacterStat);
};
