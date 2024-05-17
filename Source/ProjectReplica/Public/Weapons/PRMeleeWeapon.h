// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Weapons/PRBaseWeapon.h"
#include "PRMeleeWeapon.generated.h"

/**
 * 근접 무기 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRMeleeWeapon : public APRBaseWeapon
{
	GENERATED_BODY()

public:
	APRMeleeWeapon();
};
