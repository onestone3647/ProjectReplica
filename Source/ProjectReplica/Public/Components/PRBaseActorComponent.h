// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PRBaseActorComponent.generated.h"

class APRBaseCharacter;
class UProjectReplicaGameInstance;
class AProjectReplicaGameMode;

/**
 * PRBaseCharacter가 사용하는 ActorComponent의 부모 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRBaseActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPRBaseActorComponent();

protected:
	/** ActorComponent가 Actor에 등록될 때 호출되는 함수입니다. */
	virtual void OnRegister() override;

protected:
	/** PROwner를 초기화하는 함수입니다. */
	void InitializePROwner();

	/**
	 * ProjectReplicaGameInstance를 반환하는 함수입니다.
	 * PROwner가 월드에 속해야합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseActorComponent|PRGameInstance")
	class UProjectReplicaGameInstance* GetPRGameInstance() const;

	/**
	 * ProjectReplicaGameMode를 반환하는 함수입니다.
	 * PROwner가 월드에 속해야합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseActorComponent|PRGameMode")
	class AProjectReplicaGameMode* GetPRGameMode() const;

private:
	/** 이 ActorComponent를 소유하고 있는 PRBaseCharacter입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseActorComponent|PRCharacterReference", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class APRBaseCharacter> PROwner;

public:
	/** PROwner를 반환하는 함수입니다. */
	virtual class APRBaseCharacter* GetPROwner() const;
};
