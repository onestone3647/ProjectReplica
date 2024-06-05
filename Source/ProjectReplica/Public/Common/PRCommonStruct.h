// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PRCommonEnum.h"
#include "PRCommonStruct.generated.h"

/**
 * 프로젝트에서 공용으로 사용하는 구조체를 정의한 파일입니다.
 */

/**
 * 활성화된 Index를 보관하는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateIndexList
{
	GENERATED_BODY()

public:
	/** 활성화된 인덱스를 보관하는 Set입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActivateIndexList")
	TSet<int32> Indexes;
};

/**
 * 이전에 사용된 Index를 보관하는 구조체입니다. 
 */
USTRUCT(Atomic, BlueprintType)
struct FPRUsedIndexList
{
	GENERATED_BODY()

public:
	/** 이전에 사용된 Index를 보관하는 Set입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UsedIndexList")
	TSet<int32> Indexes;
};

/**
 * 액터 배열을 보관하는 구조체입니다. 
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActorArray
{
	GENERATED_BODY()
	
public:
	FPRActorArray()
		: Actors()
	{}

	FPRActorArray(const TArray<TObjectPtr<AActor>>& NewActors)
		: Actors(NewActors)
	{}
	
public:
	/** 액터를 보관한 배열입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectArray")
	TArray<TObjectPtr<AActor>> Actors;
};

/** 대미지 정보를 나타내는 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRDamageInfo
{
	GENERATED_BODY()

public:
	FPRDamageInfo()
		: Amount(0.0f)
		, DamageType(EPRDamageType::DamageType_None)
		, DamageElementType(EPRElementType::ElementType_None)
		, DamageResponse(EPRDamageResponse::DamageResponse_None)
		, ImpactLocation(FVector::ZeroVector)
		, bIsCritical(false)
		, bShouldDamageInvincible(false)
		, bCanBeBlocked(false)
		, bCanBeParried(false)
		, bShouldForceInterrupt(false)
	{}
   
    FPRDamageInfo(float NewAmount, EPRDamageType NewDamageType, EPRElementType NewDamageElementType, EPRDamageResponse NewEPRDamageResponse, FVector NewImpactLocation,
    				bool bNewIsCritical, bool bNewShouldDamageInvincible, bool bNewCanBeBlocked, bool bNewCanBeParried, bool bNewShouldForceInterrupt)
		: Amount(NewAmount)
		, DamageType(NewDamageType)
		, DamageElementType(NewDamageElementType)
		, DamageResponse(NewEPRDamageResponse)
		, ImpactLocation(NewImpactLocation)
		, bIsCritical(bNewIsCritical)
		, bShouldDamageInvincible(bNewShouldDamageInvincible)
		, bCanBeBlocked(bNewCanBeBlocked)
		, bCanBeParried(bNewCanBeParried)
		, bShouldForceInterrupt(bNewShouldForceInterrupt)
    {}
   	
public:
	/** 대미지 양입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	float Amount;

	/** 대미지 유형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	EPRDamageType DamageType;

	/** 대미지 속성입니다.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	EPRElementType DamageElementType;
	
	/** 대미지에 대한 반응입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	EPRDamageResponse DamageResponse;

	/** 대미지를 받은 위치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	FVector ImpactLocation;

	/** 치명타인지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	bool bIsCritical;
	
	/**
	 * 무적상태에도 대미지를 입히는지 나타내는 변수입니다.
	 * true일 경우 무적상태일 때도 대미지를 입힙니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	bool bShouldDamageInvincible;
	
	/** 방어 가능 여부를 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	bool bCanBeBlocked;
	
	/** 패링 가능 여부를 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	bool bCanBeParried;
	
	/**
	 * 동작 강제 중단 여부를 나타내는 변수입니다.
	 * ture일 경우 상대의 동작을 강제로 중단합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageInfo")
	bool bShouldForceInterrupt;
};

/**
 * 속성에 해당하는 색상을 나타내는 구조체입니다. 
 */
USTRUCT(Atomic, BlueprintType)
struct FPRElementColor : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRElementColor()
	: ElementType(EPRElementType::ElementType_None)
	, Color(FLinearColor::White)
	{}

	FPRElementColor(EPRElementType NewElementType, FLinearColor NewColor)
	: ElementType(NewElementType)
	, Color(NewColor)
	{}

public:
	/** 속성입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElementColor")
	EPRElementType ElementType;
	
	/** 속성에 해당하는 색상입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElementColor")
	FLinearColor Color;
};

/** 무기의 능력치를 나타내는 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRWeaponStat
{
	GENERATED_BODY()

public:
	FPRWeaponStat()
		: DefaultAttack(0.0f)
		, SubStatType(EPRStatType::StatType_None)
		, SubStatAmount(0.0f)
	{}
   
	FPRWeaponStat(float NewDefaultAttack, EPRStatType NewSubStatType, float NewSubStatAmount)
		: DefaultAttack(NewDefaultAttack)
		, SubStatType(NewSubStatType)
		, SubStatAmount(NewSubStatAmount)
	{}
   	
public:
	/** 무기의 기초 공격력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	float DefaultAttack;

	/** 무기의 부 옵션입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	EPRStatType SubStatType;

	/** 무기의 부 옵션 수치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	float SubStatAmount;

public:
	/**
	 * 인자로 받은 WeaponStat과 같은지 판별하는 ==연산자 오버로딩입니다.
	 * 
	 * @param NewWeaponStat 비교하는 WeaponStat과 같은지 판별할 WeaponStat입니다.
	 * @return 인자로 받은 WeaponStat과 같을 경우 true를 다를 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator==(const FPRWeaponStat& NewWeaponStat) const
	{
		return this->DefaultAttack == NewWeaponStat.DefaultAttack
				&& this->SubStatType == NewWeaponStat.SubStatType
				&& this->SubStatAmount == NewWeaponStat.SubStatAmount;
	}

	/**
	 * 인자로 받은 WeaponStat과 다른지 판별하는 !=연산자 오버로딩입니다.
	 * 
	 * @param NewWeaponStat 비교하는 WeaponStat과 같은지 판별할 WeaponStat입니다.
	 * @return 인자로 받은 WeaponStat과 다를 경우 true를 같을 경우 false를 반환합니다.
	 */
	FORCEINLINE bool operator!=(const FPRWeaponStat& NewWeaponStat) const
	{
		return this->DefaultAttack != NewWeaponStat.DefaultAttack
				|| this->SubStatType != NewWeaponStat.SubStatType
				|| this->SubStatAmount != NewWeaponStat.SubStatAmount;
	}
};

/** 무기의 정보를 나타내는 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRWeaponInfo()
		: WeaponClass(nullptr)
		, WeaponStat(FPRWeaponStat())
	{}
   
	FPRWeaponInfo(TSubclassOf<class APRBaseWeapon> NewWeaponClass, FPRWeaponStat NewWeaponStat)
		: WeaponClass(NewWeaponClass)
		, WeaponStat(NewWeaponStat)
	{}
   	
public:
	/** Spawn할 무기의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	TSubclassOf<class APRBaseWeapon> WeaponClass;

	/** 무기 능력치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	FPRWeaponStat WeaponStat;
};

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
		, UltimateRechargeRate(100.0f)
	{}

	FPRCharacterStat(float NewMaxHealth, float NewHealth, float NewAttackPoint, float NewDefencePoint, float NewCriticalRate,
						float NewCriticalDamage, float NewUltimateRechargeRate)
		: MaxHealth(NewMaxHealth)
		, Health(NewHealth)
		, AttackPoint(NewAttackPoint)
		, DefencePoint(NewDefencePoint)
		, CriticalRate(NewCriticalRate)
		, CriticalDamage(NewCriticalDamage)
		, UltimateRechargeRate(NewUltimateRechargeRate)
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (ClampMin = "15.0", ClampMax = "100.0"))
	float CriticalRate;

	/** 캐릭터의 치명타 피해량입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (ClampMin = "150.0", ClampMax = "300.0"))
	float CriticalDamage;

	/** 궁극기 충전 효율 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStat", meta = (ClampMin = "100.0", ClampMax = "300.0"))
	float UltimateRechargeRate;

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
				&& this->CriticalDamage == NewPRCharacterStat.CriticalDamage
				&& this->UltimateRechargeRate == NewPRCharacterStat.UltimateRechargeRate;
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
				|| this->CriticalDamage != NewPRCharacterStat.CriticalDamage
				|| this->UltimateRechargeRate != NewPRCharacterStat.UltimateRechargeRate;
	}
};

/**
 * 캐릭터의 레벨에 해당하는 캐릭터의 능력치를 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRLevelToCharacterStat
{
	GENERATED_BODY()

public:
	/** 레벨에 해당하는 캐릭터의 능력치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterStat")
	TMap<int32, FPRCharacterStat> CharacterStats;
};

/**
 * 캐릭터의 레벨에 해당하는 캐릭터의 능력치의 설정 값을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRLevelToCharacterStatSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** 캐릭터의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterStat")
	TSubclassOf<class APRBaseCharacter> CharacterClass;

	/** 레벨에 해당하는 캐릭터의 능력치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterStat")
	FPRLevelToCharacterStat LevelToCharacterStats;
};
