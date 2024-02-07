// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PRCommonEnum.generated.h"

/**
 * 프로젝트에서 공용으로 사용하는 열거형을 정의한 파일입니다.
 */

/**
 * Enum과 관련된 함수를 구현한 Library 클래스입니다.
 */
class PROJECTREPLICA_API PRCommonEnum
{
public:
	/**
	 * Enum 인자와 같은 이름의 Enum을 찾고 EnumValue 인자에 해당하는 값을 찾아 해당 값의 DisplayName을 String으로 변환합니다.
	 * @param Enum: 찾으려는 Enum의 이름입니다. TEXT("") 형식으로 입력합니다.
	 * @param EnumValue: 찾으려는 Enum에서 불러올 값입니다.
	 * @return String으로 변환한 Enum의 값의 DisplayName입니다. 
	 * ex)  PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	 *		    PR_LOG_SCREEN("Gait: %s", *EnumFunctionLibrary->GetEnumToString(TEXT("EPRGait"), static_cast<uint8>(Gait)));
	 *
	 *		    or
	 *
	 *		    PR_LOG_SCREEN("Gait: %s", *PRCommonEnum::GetEnumDisplayNameToString(TEXT("EPRGait"), static_cast<uint8>(Gait)));
	 */
	static FString GetEnumDisplayNameToString(const TCHAR* Enum, int32 EnumValue = 0);
};

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
