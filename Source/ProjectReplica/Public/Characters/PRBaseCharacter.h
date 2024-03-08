// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Common/PRCommonStruct.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interface_PRDamageable.h"
#include "PRBaseCharacter.generated.h"

class UPRDamageSystemComponent;
class UPRStatSystemComponent;
class UPRStateSystemComponent;
class UPRObjectPoolSystemComponent;
class UPRMovementSystemComponent;

// 임시
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * 캐릭터 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRBaseCharacter : public ACharacter, public IInterface_PRDamageable
{
	GENERATED_BODY()

public:
	APRBaseCharacter();

protected:
	/** 액터에 속한 모든 컴포넌트의 세팅이 완료되면 호출되는 함수입니다. */
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

#pragma region Interface_Damageable
public:
	/** 현재 체력을 반환하는 함수입니다. */
	virtual float GetCurrentHealth_Implementation() override;

	/** 최대 체력을 반환하는 함수입니다. */
	virtual float GetMaxHealth_Implementation() override;

	/**
	 * 입력받은 인자만큼 현재 체력을 회복하는 함수입니다.
	 *
	 * @param Amount 회복량
	 * @return 회복한 후의 체력
	 */
	virtual float Heal_Implementation(float Amount) override;

	/**
	 * 대미지를 받는 함수입니다.
	 *
	 * @param DamageInfo 대미지의 정보
	 * @return 대미지 적용 여부
	 */
	virtual bool TakeDamage_Implementation(FPRDamageInfo DamageInfo) override;
#pragma endregion

#pragma region DamageSystem
public:
	/** 캐릭터가 사망했을 때 호출하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DamagaSystem")
	virtual void Death();

	/** 캐릭터가 방어했을 때 호출하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DamagaSystem")
	virtual void Blocked(bool bCanBeParried);

	/** 캐릭터의 대미지에 대한 반응을 호출하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DamagaSystem")
	virtual void DamageResponse(EPRDamageResponse DamageResponse);

	/** 캐릭터가 대미지를 주는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "DamagaSystem")
	virtual void DoDamage();	
	
private:
	/** 대미지를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPRDamageSystemComponent> DamageSystem;

	// 임시
	/** DamageSystem의 디버그의 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "임시", meta = (AllowPrivateAccess = "true"))
	bool bDamageSystemDebug;

	// 임시
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "임시", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> HitNiagaraEffect;

	// 임시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "임시", meta = (AllowPrivateAccess = "true"))
	EPRElement DamageElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "임시", meta = (AllowPrivateAccess = "true"))
	float DamageAmount;
	
public:
	/** DamageSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRDamageSystemComponent* GetDamageSystem() const { return DamageSystem; }
#pragma endregion

#pragma region StatSystem
private:
	/** 캐릭터의 능력치를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPRStatSystemComponent> StatSystem;

public:
	/** StatSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRStatSystemComponent* GetStatSystem() const { return StatSystem; }
#pragma endregion

#pragma region StateSystem
public:
	/** 캐릭터가 사망했는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StateSystem")
	bool IsDead() const;

	/** 캐릭터가 무적 상태인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StateSystem")
	bool IsInvincible() const;

	/** 캐릭터가 방어 상태인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StateSystem")
	bool IsBlocking() const;
	
private:
	/** 캐릭터의 상태(State)를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPRStateSystemComponent> StateSystem;

public:
	/** StateSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRStateSystemComponent* GetStateSystem() const { return StateSystem; }
#pragma endregion

#pragma region ObjectPoolSystem
private:
	/** 오브젝트 풀을 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPRObjectPoolSystemComponent> ObjectPoolSystem;

public:
	/** ObjectPoolSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRObjectPoolSystemComponent* GetObjectPoolSystem() const { return ObjectPoolSystem; }
#pragma endregion 

#pragma region MovementSystem
private:
	/** 캐릭터의 움직임에 관련된 정보를 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPRMovementSystemComponent> MovementSystem;

public:
	/** MovementSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRMovementSystemComponent* GetMovementSystem() const { return MovementSystem; }
#pragma endregion

#pragma region Locomotion
public:
	/** 캐릭터의 Locomotion을 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "Locomotion")
	void ToggleWalk();
#pragma endregion
};
