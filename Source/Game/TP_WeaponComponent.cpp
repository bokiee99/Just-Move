// Copyright Epic Games, Inc. All Rights Reserved.
// 기본 언리얼 1인칭 게임 테플릿 활용용

#include "TP_WeaponComponent.h"
#include "GameCharacter.h"
#include "GameProjectile.h"
#include "GameDoor.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GamePlatform.h"
#include "DrawDebugHelpers.h" 
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "LaserBeam.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent() 
{
	//소켓 할당
	MuzzleOffset1 = FVector(100.0f, 0.0f, 10.0f);
	MuzzleOffset2 = FVector(60.0f, 0.0f, 15.0f);

	MaxImpulse = 1500000.0f;

	//LaserBeamEffect = ObjectInitializer.CreateDefaultSubobject<UNiagaraComponent>(this, TEXT("LaserBeamEffect"));
    //LaserBeamEffect->SetAutoActivate(false);
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr || Character-> GetCrouching())
	{
		return;
	}

	// 기본 총알 발사 (무기 1번 기능)
	if (ProjectileClass != nullptr && Character->GetType())
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset1);

			//DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation+1000, FColor::Green, true, 2.0f, 0, 2.0f);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<AGameProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
			}
		}
	}

	//레이저 빔 발사 (무기 2번 기능)
	if(!(Character->GetType()))
	{
		UWorld* const World = GetWorld();
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset2);
		//레이저 시작점점
		FVector Start = SpawnLocation;

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		
		//레이저 끝점
		FVector End = Start + (GetRightVector() * 1000.f);
		FHitResult HitResult;

		AActor* LaserBeam = World->SpawnActor<ALaserBeam>(LaserBeamEffectClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (LaserSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LaserSound, Character->GetActorLocation());
		}
		
		// 디버그 라인 출력
   		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 2.0f, 0, 2.0f);

    	//LaserBeamEffect->ActivateSystem();

		if (LaserBeam)
        {
            ALaserBeam* LaserBeamActor = Cast<ALaserBeam>(LaserBeam);
            if (LaserBeamActor)
            {
                LaserBeamActor->ActivateLaserBeam(Start, End);
                LaserBeamActor->SetLifeSpan(0.1f);  // 0.5초 후 소멸
            }
        }

		/*if (LaserBeamEffect) 
    	{
        	//LaserBeamEffect->SetVectorParameter("Beam Start", Start);  // 나이아가라 파라미터 설정
        	//LaserBeamEffect->SetVectorParameter("Beam End", End);

			//GetWorld()->GetTimerManager().SetTimer(LaserBeamTimerHandle, this, &UTP_WeaponComponent::DeactivateLaserBeam, 0.1f, false);
    	}*/

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
    	{
        	AActor* HitActor = HitResult.GetActor();
        	UE_LOG(LogTemp, Display, TEXT("Hit Actor with %s"), *HitActor->GetClass()->GetName());
			
			//플랫폼 히트 시시
        	if (HitActor && HitActor->IsA(AGamePlatform::StaticClass()))
        	{
        	    UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(HitActor->GetRootComponent());
				AGamePlatform* HitPlatform = Cast<AGamePlatform>(HitActor);
            	if (MeshComp)
            	{
    	            MeshComp->SetSimulatePhysics(true);

					//캐릭터 어태티 및 오버랩 여부 확인 
					if(!Character->IsAttachedTo(HitPlatform) && HitPlatform->GetOverlapped())
					{
						Character->AttachToActor(HitPlatform, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
					}

					UE_LOG(LogTemp, Warning, TEXT("Hit!!! %f"), HitPlatform->GetStoredImpulse().Size());
					//최대 충격량 초과 시 파괴괴
					if(HitPlatform->GetStoredImpulse().Size() > MaxImpulse) 
					{
						if (boomEffect && GetWorld())
                    	{
                            FVector Location = HitPlatform->GetActorLocation();
                            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), boomEffect, Location, FRotator::ZeroRotator, true);
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, Location);
                    	}
						UE_LOG(LogTemp, Warning, TEXT("Hit!!! %f"), HitPlatform->GetStoredImpulse().Size());
						HitPlatform->Destroy();
					}
					else 
					{
						//누적된 충격량 부여 및 플랫폼 수명 시간 설정
						HitPlatform->SetImpulse(HitPlatform->GetStoredImpulse());
						HitPlatform->SetLifeSpan(3.0f);

						FTimerHandle ParticleSpawnTimerHandle;
    					GetWorld()->GetTimerManager().SetTimer(ParticleSpawnTimerHandle, FTimerDelegate::CreateLambda([this, HitPlatform]()
    					{
							HitPlatform->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
        					if (boomEffect && GetWorld())
        					{
            					FVector Location = HitPlatform->GetActorLocation();
            					// 파티클을 플랫폼 위치에 생성
            					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), boomEffect, Location, FRotator::ZeroRotator, true);
        					}

							if (DestroySound && GetWorld())  // 파괴 사운드 재생
    						{
        						FVector Location = HitPlatform->GetActorLocation();
        						UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, Location);
    						}
    					}), 3.0f, false);
					}
        	    }
        	}
			
			//문에 히트 시
			if (HitActor && HitActor->IsA(AGameDoor::StaticClass()))
			{
				//UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(HitActor->GetRootComponent());
				AGameDoor* HitDoor = Cast<AGameDoor>(HitActor);

				//문 활성화
				HitDoor->SetDoorState(true);

				//5초 후 문 비활성화화
				FTimerHandle DoorTimeHandle;
				GetWorld()->GetTimerManager().SetTimer(DoorTimeHandle, [HitDoor]()
            	{
                	if (HitDoor)
                	{
                    	HitDoor->SetDoorState(false);  // 5초 후 false로 설정
                	}
            	}, 5.0f, false);
			}
    	}
	}
	
	// Try and play the sound if specified
	/*if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}*/
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

//기본 템플릿 함수 그대로 사용용
bool UTP_WeaponComponent::AttachWeapon(AGameCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	AActor* OwnerActor = GetOwner();  // GetOwner() will return the owner actor of this component
    if (OwnerActor)
    {
        OwnerActor->SetOwner(Character);  // Set the owner of the weapon component's owner to the character
    }

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

/*void UTP_WeaponComponent::DeactivateLaserBeam()
{
    if (LaserBeamEffect)
    {
        LaserBeamEffect->Deactivate();
    }
}*/
