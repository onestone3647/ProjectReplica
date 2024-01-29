// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/PlayerController.h"
#include "PRPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREPLICA_API APRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APRPlayerController();

protected:
	virtual void BeginPlay() override;
};
