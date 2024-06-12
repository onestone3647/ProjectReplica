// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRFootsteps.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UAN_PRFootsteps::UAN_PRFootsteps(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Debug
	bDebug = false;
	
	TraceDistance = 150.0f;
}

void UAN_PRFootsteps::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	PlayFootsteps(MeshComp);
	ActivateFootstepsEffect(MeshComp);
}

void UAN_PRFootsteps::PlayFootsteps(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner) && PROwner->GetFootstepsSound())
		{
			FHitResult HitResult;
			const FVector TraceStart = MeshComp->GetOwner()->GetActorLocation();
			const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, TraceDistance);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(MeshComp->GetOwner());

			// 디버그 옵션을 설정합니다.
			EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
			if(bDebug)
			{
				DebugType = EDrawDebugTrace::ForDuration;
			}

			bool bIsHit = UKismetSystemLibrary::LineTraceSingle(MeshComp->GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_Visibility),
																true, ActorsToIgnore, DebugType, HitResult, true);
			if(bIsHit)
			{
				UAudioComponent* FootstepsAudioComp = UGameplayStatics::SpawnSoundAtLocation(MeshComp->GetWorld(), PROwner->GetFootstepsSound(), HitResult.Location);
				if(IsValid(FootstepsAudioComp))
				{
					FootstepsAudioComp->Play();
					// FootstepsAudioComp->SetIntParameter(TEXT("Gender"), static_cast<int32>(PROwner->GetGender()));
					FootstepsAudioComp->SetIntParameter(TEXT("SurfaceType"), static_cast<int32>(UGameplayStatics::GetSurfaceType(HitResult)));
					// FootstepAudioComp->SetFloatParameter(TEXT("VolumeMultiplier"), VolumeMultiplier);
					// FootstepAudioComp->SetFloatParameter(TEXT("PitchMultiplier"), PitchMultiplier);
					// FootstepAudioComp->SetBoolParameter(TEXT("PlayFootstep"), bPlayFootstep);
				}
			}
		}
	}
}

void UAN_PRFootsteps::ActivateFootstepsEffect(USkeletalMeshComponent* MeshComp)
{
}
