// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "PRBaseWeapon.generated.h"

class APRBaseCharacter;
class UNiagaraSystem;

/** 무기의 종류를 나타내는 열거형입니다. */
UENUM(BlueprintType)
enum class EPRWeaponType : uint8
{
	WeaponType_None				UMETA(DisplayName = "None"),
	WeaponType_Sword		   	UMETA(DisplayName = "Sword"),			// 한손검
	WeaponType_DualSword		UMETA(DisplayName = "DualSword"),		// 이도류
	WeaponType_Claymore 	  	UMETA(DisplayName = "Claymore"),		// 양손검
	WeaponType_Orb				UMETA(DisplayName = "Orb"),				// 오브
	WeaponType_Bow				UMETA(DisplayName = "Bow")				// 활
};

/**
 * 캐릭터가 사용하는 무기의 기본 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	APRBaseWeapon();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * 무기를 초기화하는 함수입니다.
	 *
	 * @param NewPROwner 무기의 소유자
	 * @param NewWeaponStat 무기의 능력치
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	virtual void InitializeWeapon(class APRBaseCharacter* NewPROwner, FPRWeaponStat NewWeaponStat);	
	
	/** 무기를 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	virtual void Activate();

	/** 무기를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	virtual void Deactivate();

	/** 무기가 활성화되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	bool IsActivate() const;

	/**
	 * 무기를 발도하는 함수입니다.
	 *
	 * @param bActivateSpawnEffect Spawn이펙트 활성화 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	virtual void Draw(bool bActivateSpawnEffect = false);

	/**
	 * 무기를 납도하는 함수입니다.
	 * 
 	 * @param bActivateSpawnEffect Spawn이펙트 활성화 여부
 	 * @param bVisible 무기 숨김 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	virtual void Sheathe(bool bActivateSpawnEffect = false, bool bVisible = true);

protected:
	/** 무기의 RootComponent입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "PRBaseWeapon")
	USceneComponent* Root;
	
	/** 무기의 종류입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon")
	EPRWeaponType WeaponType;

	/** 무기의 능력치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon")
	FPRWeaponStat WeaponStat;

private:
	/** 무기의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon", meta = (AllowPrivateAccess = "true"))
	bool bActivate;	
	
	/** 무기의 소유자입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APRBaseCharacter> PROwner;

public:
	/** PROwner를 반환하는 함수입니다. */
	class APRBaseCharacter* GetPROwner() const;

#pragma region SpawnEffect
protected:
	/** 무기의 Spawn 이펙트입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SpawnEffect")
	TObjectPtr<UNiagaraSystem> SpawnNiagaraEffect;

	/** 무기의 Spawn 이펙트의 색상입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SpawnEffect")
	FLinearColor EffectColor;

	/** SpawnNiagaraEffect의 사용자 파라미터인 WeaponMesh의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SpawnEffect")
	FName WeaponMeshVariableName;

	/** SpawnNiagaraEffect의 사용자 파라미터인 EffectColor의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|SpawnEffect")
	FName EffectColorVariableName;
#pragma endregion 

#pragma region MainWeapon
protected:
	/** 메인 무기입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	TObjectPtr<USceneComponent> MainWeapon;

	/** 메인 무기의 외형입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	TObjectPtr<UStaticMeshComponent> MainWeaponMesh;

	/** 메인 무기를 발도하는 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	FName MainWeaponDrawSocketName;

	/** 메인 무기를 납도하는 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	FName MainWeaponSheatheSocketName;
	
	/** 메인 무기의 발도 위치 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	FVector MainWeaponDrawLocationOffset;

	/** 메인 무기의 발도 회전 값 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	FRotator MainWeaponDrawRotationOffset;

	/** 메인 무기의 납도 위치 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	FVector MainWeaponSheatheLocationOffset;

	/** 메인 무기의 납도 회전 값 Offset입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|MainWeapon")
	FRotator MainWeaponSheatheRotationOffset;
#pragma endregion
};
