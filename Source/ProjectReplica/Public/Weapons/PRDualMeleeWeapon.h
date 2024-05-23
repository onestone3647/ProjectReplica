// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Weapons/PRMeleeWeapon.h"
#include "PRDualMeleeWeapon.generated.h"

/**
 * 양손에 하나씩 사용하는 근접 무기 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRDualMeleeWeapon : public APRMeleeWeapon
{
	GENERATED_BODY()

public:
	APRDualMeleeWeapon();
	
public:
	/**
	 * 무기를 발도하는 함수입니다.
	 *
	 * @param bActivateSpawnEffect Spawn 이펙트 활성화 여부
	 */
	virtual void Draw(bool bActivateSpawnEffect = false) override;

	/**
	 * 무기를 납도하는 함수입니다.
	 * 
	  * @param bActivateSpawnEffect Spawn 이펙트 활성화 여부
	  * @param bVisible 무기 숨김 여부
	 */
	virtual void Sheathe(bool bActivateSpawnEffect = false, bool bVisible = true) override;

#pragma region SubWeapon
protected:
	/** 서브 무기입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	TObjectPtr<USceneComponent> SubWeapon;
	
	/** 서브 무기의 외형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	TObjectPtr<UStaticMeshComponent> SubWeaponMesh;

	/** 서브 무기를 발도하는 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	FName SubWeaponDrawSocketName;

	/** 서브 무기를 납도하는 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	FName SubWeaponSheatheSocketName;
	
	/** 서브 무기의 발도 위치 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	FVector SubWeaponDrawLocationOffset;

	/** 서브 무기의 발도 회전 값 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	FRotator SubWeaponDrawRotationOffset;

	/** 서브 무기의 납도 위치 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	FVector SubWeaponSheatheLocationOffset;

	/** 서브 무기의 납도 회전 값 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SubWeapon")
	FRotator SubWeaponSheatheRotationOffset;
#pragma endregion 
};
