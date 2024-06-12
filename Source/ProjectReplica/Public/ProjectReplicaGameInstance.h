// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Engine/GameInstance.h"
#include "ProjectReplicaGameInstance.generated.h"

class APRBaseCharacter;
class APRBaseWeapon;
class UPRStatSystemComponent;

/**
 * 게임의 인스턴스 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UProjectReplicaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UProjectReplicaGameInstance();

public:
	virtual void Init() override;

#pragma region ElementColor
public:
	/** 속성에 해당하는 색상을 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "ElementColor")
	FLinearColor GetElementColor(EPRElementType ElementType) const;
	
private:
	/** ElementColorDataTable의 정보로 ElementColors를 초기화하는 함수입니다. */
	void InitializeElementColors();
	
private:
	/** 속성의 색상을 가진 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ElementColor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ElementColorDataTable;
	
	/** 속성에 해당하는 색상을 보관한 Map입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElementColor", meta = (AllowPrivateAccess = "true"))
	TMap<EPRElementType, FLinearColor> ElementColors;
#pragma endregion

#pragma region WeaponInfo
public:
	/** 인자에 해당하는 무기의 능력치를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "WeaponInfo")
	FPRWeaponStat GetWeaponStat(TSubclassOf<class APRBaseWeapon> NewWeapon) const;
	
private:
	/** WeaponInfoDataTable의 정보로 WeaponInfos를 초기화하는 함수입니다. */
	void InitializeWeaponInfo();
	
private:
	/** 무기의 정보를 가진 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> WeaponInfoDataTable;

	/** 무기에 해당하는 능력치를 보관한 Map입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<class APRBaseWeapon>, FPRWeaponStat> WeaponInfos;

public:
	/** WeaponInfos를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "WeaponInfo")
	TMap<TSubclassOf<class APRBaseWeapon>, FPRWeaponStat> GetWeaponInfos() const;
#pragma endregion

#pragma region CharacterStat
public:
	/** 인자로 받은 캐릭터의 레벨에 해당하는 능력치를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CharacterStat")
	FPRCharacterStat GetCharacterStat(TSubclassOf<class APRBaseCharacter> NewCharacter, int32 NewLevel) const;
	
private:
	/** CharacterStatInfoDataTable의 정보로 CharacterStatSettings를 초기화하는 함수입니다. */
	void InitializeCharacterStatSettings();
	
private:
	/** 캐릭터의 능력치의 설정 값을 가진 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> CharacterStatSettingsDataTable;

	/** 캐릭터에 해당하는 능력치의 설정 값을 보관한 Map입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterStat", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<class APRBaseCharacter>, FPRLevelToCharacterStat> CharacterStatSettings;

public:
	/** CharacterStatSettings를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "CharacterStat")
	TMap<TSubclassOf<class APRBaseCharacter>, FPRLevelToCharacterStat> GetCharacterStatSettings() const;
#pragma endregion 
};
