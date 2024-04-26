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
		, DamageElement(EPRElement::Element_None)
		, DamageResponse(EPRDamageResponse::DamageResponse_None)
		, ImpactLocation(FVector::ZeroVector)
		, bIsCritical(false)
		, bShouldDamageInvincible(false)
		, bCanBeBlocked(false)
		, bCanBeParried(false)
		, bShouldForceInterrupt(false)
	{}
   
    FPRDamageInfo(float NewAmount, EPRDamageType NewDamageType, EPRElement NewDamageElement, EPRDamageResponse NewEPRDamageResponse, FVector NewImpactLocation,
    				bool bNewIsCritical, bool bNewShouldDamageInvincible, bool bNewCanBeBlocked, bool bNewCanBeParried, bool bNewShouldForceInterrupt)
		: Amount(NewAmount)
		, DamageType(NewDamageType)
		, DamageElement(NewDamageElement)
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
	EPRElement DamageElement;
	
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
	: Element(EPRElement::Element_None)
	, Color(FLinearColor::White)
	{}

	FPRElementColor(EPRElement NewElement, FLinearColor NewColor)
	: Element(NewElement)
	, Color(NewColor)
	{}

public:
	/** 속성입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElementColor")
	EPRElement Element;
	
	/** 속성에 해당하는 색상입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ElementColor")
	FLinearColor Color;
};
