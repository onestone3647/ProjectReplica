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
 * 활성화된 Index를 보관한 목록을 나타내는 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRActivateIndexList
{
	GENERATED_BODY()

public:
	/** 활성화된 Index를 보관한 Set입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActivateIndexList")
	TSet<int32> Indexes;
};

/**
 * 이전에 사용된 Index를 보관한 목록을 나타내는 구조체입니다. 
 */
USTRUCT(Atomic, BlueprintType)
struct FPRUsedIndexList
{
	GENERATED_BODY()

public:
	/** 이전에 사용된 Index를 추적하기 위한 Set입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UsedIndexList")
	TSet<int32> Indexes;
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
