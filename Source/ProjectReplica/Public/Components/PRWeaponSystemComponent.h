// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Weapons/PRBaseWeapon.h"
#include "PRWeaponSystemComponent.generated.h"

/** 무기의 상태를 나타내는 열거형입니다. */
UENUM(BlueprintType)
enum class EPRWeaponState : uint8
{
	WeaponState_None		UMETA(DisplayName = "None"),
	WeaponState_Draw		UMETA(DisplayName = "Draw"),		// 발도
	WeaponState_Sheathe		UMETA(DisplayName = "Sheathe")		// 납도
};

/**
 * 캐릭터가 사용하는 무기를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRWeaponSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRWeaponSystemComponent();

public:
	/** WeaponSystem을 초기화하는 함수입니다. 장착할 무기를 월드에 Spawn하고 장착합니다. */
	void InitializeWeaponSystem();
	
	/**
	 * 장비한 무기를 발도하는 함수입니다.
	 *
	 * @param bSpawnActivateEffect Spawn 이펙트 실행 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	void DrawWeapon(bool bSpawnActivateEffect = false);

	/**
	 * 장비한 무기를 납도하는 함수입니다.
	 * 
 	 * @param bSpawnActivateEffect Spawn 이펙트 실행 여부
 	 * @param bVisible 무기 숨김 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	void SheatheWeapon(bool bSpawnActivateEffect = false, bool bVisible = true);

	/** 무기를 장착하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	APRBaseWeapon* EquipWeapon(TSubclassOf<APRBaseWeapon> NewWeaponClass);

	/** 무기를 장착 해제하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	void UnequipWeapon();

	/** 무기가 발도 상태인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	bool IsDrawWeapon() const;

	/** 무기가 납도 상태인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	bool IsSheatheWeapon() const;

protected:
	/** 인자로 받은 PRBaseWeapon 클래스 레퍼런스로 월드에 무기를 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	APRBaseWeapon* SpawnWeaponInWorld(TSubclassOf<class APRBaseWeapon> SpawnWeaponClass);

private:
	/** 장착한 무기의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APRBaseWeapon> EquippedWeaponClass;
	
	/** 무기의 상태를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	EPRWeaponState WeaponState;
	
	/** 장착한 무기입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APRBaseWeapon> EquippedWeapon;

public:
	/** EquippedWeapon을 반환하는 함수입니다. */
	class APRBaseWeapon* GetEquippedWeapon() const;
};
