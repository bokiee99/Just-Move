// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameCharacter.h"
#include "GameProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGameCharacter

AGameCharacter::AGameCharacter()
{
	bHasRifle = false;
	bCrouching = false;
	bRunning = false;
	bIsType = true;
	bToggleFootstep = true;

	StandingCapsuleHalfHeight = 96;
	CrouchingCapsuleHalfHeight = 48;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	CrouchedCameraOffset = 80.0f;
	CameraInterpSpeed = 20.0f;
	TargetCameraLocation = FirstPersonCameraComponent->GetRelativeLocation();

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	FootstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComponent"));
    FootstepAudioComponent->bAutoActivate = false; // 자동 재생하지 않도록 설정
    FootstepAudioComponent->SetupAttachment(RootComponent); // 캐릭터에 어태치

}

void AGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

//////////////////////////////////////////////////////////////////////////// Input
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCameraLocation(DeltaTime);
	InterpCapsuleHalfHeight(DeltaTime);

	AActor* ParentActor = GetAttachParentActor();
	if(ParentActor)
	{
		StopFootstepLoop();
	}
	
	FVector Velocity = GetVelocity();
	if (!GetCharacterMovement()->IsFalling() && Velocity.Size() > 0)  // 캐릭터가 이동 중인지 확인
    {
		float FootstepInterval = 0.7f;

		if(bCrouching) FootstepInterval = 0.9f;
		if(bRunning) FootstepInterval = 0.5f;


        if (!GetWorld()->GetTimerManager().IsTimerActive(FootstepLoopTimerHandle) || CurrentFootstepInterval != FootstepInterval)
        {
            StopFootstepLoop();  // 기존 타이머 정지
            StartFootstepLoop(FootstepInterval);  // 새로운 간격으로 타이머 시작
            CurrentFootstepInterval = FootstepInterval;  // 현재 간격 업데이트
        }
    }
    else  // 캐릭터가 멈춘 경우
    {
        if (GetWorld()->GetTimerManager().IsTimerActive(FootstepLoopTimerHandle))
        {
            StopFootstepLoop();  // 발걸음 소리 정지
			CurrentFootstepInterval = 0.7f;
        }
    }
}

void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameCharacter::Look);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AGameCharacter::CrouchButtonPressed);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AGameCharacter::RunButtonPressed);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AGameCharacter::RunButtonReleased);

		EnhancedInputComponent->BindAction(Type1Action, ETriggerEvent::Started, this, &AGameCharacter::Type1Pressed);
		EnhancedInputComponent->BindAction(Type2Action, ETriggerEvent::Started, this, &AGameCharacter::Type2Pressed);

		EnhancedInputComponent->BindAction(DettachAction, ETriggerEvent::Started, this, &AGameCharacter::DettachPressed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	AActor* ParentActor = GetAttachParentActor();
	if(ParentActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attached to: %s"), *ParentActor->GetName());
		DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		EMovementMode PreviousMode = GetCharacterMovement()->MovementMode;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->SetMovementMode(PreviousMode);
	}

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * MouseSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * MouseSensitivity);
	}
}

void AGameCharacter::RunButtonPressed()
{
	if(!bCrouching)
	{
		bRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 800.0f;
	}
}

void AGameCharacter::RunButtonReleased()
{
	if(!bCrouching)
	{
		bRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void AGameCharacter::Type1Pressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Your message1"));
	bIsType = true;
}

void AGameCharacter::Type2Pressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Your message2"));
	bIsType = false;
}

//현재 불필요, 상하좌우 이동으로 자연스럽게 떨어지도록 변경
void AGameCharacter::DettachPressed()
{
	//추후 검토 (이동속도 살짝 느려지는 느낌이 있음)
	UE_LOG(LogTemp, Warning, TEXT("Your message"));
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	EMovementMode PreviousMode = GetCharacterMovement()->MovementMode;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	GetCharacterMovement()->SetMovementMode(PreviousMode);
}

void AGameCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;

}

bool AGameCharacter::GetHasRifle()
{
	return bHasRifle;
}

bool AGameCharacter::GetNotCrouching()
{
	return !bCrouching;
}

bool AGameCharacter::GetBulletType()
{
	return bIsType;
}

void AGameCharacter::CrouchButtonPressed()
{
	if(!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;

		if(bCrouching)
		{
			GetCharacterMovement()->MaxWalkSpeed = 200.0f;
			TargetCameraLocation = FVector(0.0f, 0.0f, 64.0f - CrouchedCameraOffset);
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			TargetCameraLocation = FVector(0.0f, 0.0f, 64.0f);
		}
	}
}

void AGameCharacter::UpdateCameraLocation(float DeltaTime)
{
	if(FirstPersonCameraComponent)
	{
		FVector CurrentLocation = FirstPersonCameraComponent->GetRelativeLocation();

		FVector InterpLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, CameraInterpSpeed);

		FirstPersonCameraComponent->SetRelativeLocation(InterpLocation);
	}
}

void AGameCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if(bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight = FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f);

	const float DeltaCapsuleHalfHeight = InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector MeshOffset = FVector(0.f, 0.f, -DeltaCapsuleHalfHeight);
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AGameCharacter::StartFootstepLoop(float interval)
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(FootstepLoopTimerHandle))
    {
        PlayFootstepSound();
        GetWorld()->GetTimerManager().SetTimer(FootstepLoopTimerHandle, this, &AGameCharacter::PlayFootstepSound, interval, true);
    }
}

// 발걸음 소리를 멈추는 함수
void AGameCharacter::StopFootstepLoop()
{
    if (FootstepAudioComponent && FootstepAudioComponent->IsPlaying())
    {
        FootstepAudioComponent->Stop(); // 사운드 멈춤
    }

    GetWorld()->GetTimerManager().ClearTimer(FootstepLoopTimerHandle); // 타이머 정지
}

// 발걸음 소리를 재생하는 함수 (번갈아가며 재생)
void AGameCharacter::PlayFootstepSound()
{	
	UE_LOG(LogTemp, Warning, TEXT("Your message %f"), CurrentFootstepInterval);

	if (FootstepAudioComponent)
	{
    	if (bToggleFootstep)
    	{
        	FootstepAudioComponent->SetSound(FootstepSound1); // USoundBase 할당
    	}
    	else
    	{
        	FootstepAudioComponent->SetSound(FootstepSound2); // USoundBase 할당
    	}
	}

    FootstepAudioComponent->Play(); // 사운드 재생
    bToggleFootstep = !bToggleFootstep; // 사운드를 교차하여 재생
}