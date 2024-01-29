// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PRCommonEnum.generated.h"

/**
 * 프로젝트에서 공용으로 사용하는 열거형을 정의한 파일입니다.
 */

/**
 * 방향을 나타내는 열거형입니다. 
 */
UENUM(BlueprintType)
enum class EPRDirection : uint8
{
	Direction_None					UMETA(DisplayName = "None"),
	Direction_Forward		    	UMETA(DisplayName = "Forward"),
	Direction_Backward 		  		UMETA(DisplayName = "Backward"),
	Direction_Left		        	UMETA(DisplayName = "Left"),
	Direction_Right			    	UMETA(DisplayName = "Right"),
	Direction_ForwardLeft			UMETA(DisplayName = "ForwardLeft"),
	Direction_ForwardRight			UMETA(DisplayName = "ForwardRight"),
	Direction_BackwardLeft			UMETA(DisplayName = "BackwardLeft"),
	Direction_BackwardRight			UMETA(DisplayName = "BackwardRight")
};

/**
 * 대미지 유형을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRDamageType : uint8
{
	DamageType_None				UMETA(DisplayName = "None"),
	DamageType_Melee			UMETA(DisplayName = "Melee"),				// 근거리 공격
	DamageType_Projectile		UMETA(DisplayName = "Projectile"),			// 원거리(투사체) 공격
	DamageType_Explosion		UMETA(DisplayName = "Explosion"),			// 폭발
	DamageType_Environment		UMETA(DisplayName = "Environment")			// 환경 ex) 초원이 불에 탈 때 입는 화상 대미지
};

/**
 * 대미지에 대한 반응을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRDamageResponse : uint8
{
	DamageResponse_None				UMETA(DisplayName = "None"),
	DamageResponse_HitReaction		UMETA(DisplayName = "HitReaction"),			// 적중반응
	DamageResponse_Stagger			UMETA(DisplayName = "Stagger"),				// 비틀거림
	DamageResponse_Stun				UMETA(DisplayName = "Stun"),				// 스턴
	DamageResponse_KnockBack		UMETA(DisplayName = "KnockBack")			// 넉백
};

/**
 * 속성을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRElement : uint8
{
	Element_None			UMETA(DisplayName = "None"),
	Element_Physio			UMETA(DisplayName = "Physio"),			// 물리 속성
	Element_Pyro			UMETA(DisplayName = "Pyro"),			// 화염 속성
	Element_Hydro			UMETA(DisplayName = "Hydro"),			// 물 속성
	Element_Cryo			UMETA(DisplayName = "Cryo"),			// 얼음 속성
	Element_Anemo			UMETA(DisplayName = "Anemo"),			// 바람 속성
	Element_Electro			UMETA(DisplayName = "Electro"),			// 번개 속성
	Element_Geo				UMETA(DisplayName = "Geo"),				// 땅 속성
	Element_Photo			UMETA(DisplayName = "Photo"),			// 빛 속성
	Element_Erebo			UMETA(DisplayName = "Erebo")			// 어둠 속성
};
