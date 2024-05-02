// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "PRBaseWeapon.generated.h"

/** 무기의 종류를 나타내는 열거형입니다. */
UENUM(BlueprintType)
enum class EPRWeaponType : uint8
{
	WeaponType_None				UMETA(DisplayName = "None"),
	WeaponType_Sword		   	UMETA(DisplayName = "Sword"),			// 한손검
	WeaponType_Claymore 	  	UMETA(DisplayName = "Claymore"),		// 양손검
	WeaponType_Orb				UMETA(DisplayName = "Orb"),				// 오브
	WeaponType_Bow				UMETA(DisplayName = "Bow")				// 활
};

UCLASS()
class PROJECTREPLICA_API APRBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	APRBaseWeapon();

protected:
	virtual void BeginPlay() override;

};
