// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Common/PRCommonEnum.h"
#include "Common/PRCommonStruct.h"

/** 로그 카테고리를 정의합니다. */
DECLARE_LOG_CATEGORY_EXTERN(ProjectReplica, Log, All);

/**
 * FString으로 선언된 변수에서 문자열 정보를 얻어오려면 반드시 * 연산자를 앞에 지정해줘야 합니다.
 * ex) FString::Printf(TEXT("Actor Name: %s, ID: %d, Location X: %f"),
 *                      *GetName(), ID, GetActorLocation().X);
 */

/** 코드가 들어있는 파일 이름과 함수, 라인 정보를 로그로 남기는 매크로입니다. */
#define PR_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

/**
 * PR_LOG_CALLINFO와 함께 아무 내용이 없는 로그를 남기는 매크로입니다.
 * ex) PR_LOG_S(Warning);
 *     PR_LOG_S(Error);
 */
#define PR_LOG_S(Verbosity) UE_LOG(ProjectReplica, Verbosity, TEXT("%s"), *PR_LOG_CALLINFO)

/**
 * PR_LOG_CALLINFO와 함께 문자열로 추가 정보를 로그로 남기는 매크로입니다.
 * ex) PR_LOG(Warning, "Warning");
 *     PR_LOG(Error, "Error");
 */
#define PR_LOG(Verbosity, Format, ...) UE_LOG(ProjectReplica, Verbosity, TEXT("%s %s"), *PR_LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

/**
 * Screen에 로그를 밀면서 출력하는 매크로입니다.
 * ex) PR_LOG_SCREEN("Actor Name: %s, ID: %d, Location X: %f",
 *                    *GetName(), ID, GetActorLocation().X);
 */
#define PR_LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

/**
 * Screen에 로그를 Index 순서대로 고정하여 갱신하는 매크로입니다.
 * ex) PR_LOG_SCREEN_INFO(1, "Actor Name: %s, ID: %d, Location X: %f",
 *                        *GetName(), ID, GetActorLocation().X);
 */
#define PR_LOG_SCREEN_INFO(Index, Format, ...) GEngine->AddOnScreenDebugMessage(Index, 0.0f, FColor::Cyan, FString::Printf(TEXT(Format), ##__VA_ARGS__))

/** PR_LOG_SCREEN에 시간을 넣은 매크로입니다. Tick 함수에서 DeltaTime을 인자로 넣어서 사용합니다. */
#define PR_LOG_SCREEN_T(Time, Format, ...) GEngine->AssOnScreenDebugMessage(-1, Time, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

/**
 * KismetSystem의 PrintString 함수를 재정의한 매크로입니다.
 * ex) PR_LOG_SCREEN_KISMET(GetWorld(), "Actor Name: " + GetPROwner()->GetName(), 2.0f);
 */
#define PR_LOG_SCREEN_KISMET(WorldContextObject, Format, Duration, ...) UKismetSystemLibrary::PrintString(WorldContextObject, Format, true, true, FLinearColor(0.0, 0.66, 1.0), Duration);

/**
 * PR_LOG에서 Verbosity를 뺀 매크로입니다.
 * ex) PR_LOG_WARNING("Warning");
 *     PR_LOG_ERROR("Error");
 */
#define PR_LOG_WARNING(Format, ...) UE_LOG(ProjectReplica, Warning, TEXT("%s %s"), *PR_LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define PR_LOG_ERROR(Format, ...) UE_LOG(ProjectReplica, Error, TEXT("%s %s"), *PR_LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

/**
 * 런타임에서 문제가 발생했을 때 붉은 색으로 에러 로그를 뿌리고 바로 함수를 반환합니다.
 * ex) PR_CHECK(조건문);
 */
#define PR_CHECK(Expr, ...) {if(!(Expr)) {PR_LOG(Error, TEXT("ASSERTION: %s")), TEXT("'"#Expr"'")); return __VA_ARGS__;}}
