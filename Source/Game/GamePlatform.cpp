// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlatform.h"
#include "GameProjectile.h"
#include "GameCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
AGamePlatform::AGamePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    SetRootComponent(PlatformMesh);

	PlatformMesh->BodyInstance.SetCollisionProfileName("Projectile2");

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());

	//현재 AreaSphere 사용 안함 (박스로 대체)
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	StoredImpulse = FVector::ZeroVector;

}

// Called when the game starts or when spawned
void AGamePlatform::BeginPlay()
{
	Super::BeginPlay();

	//시작시 플랫폼 시뮬레이트 비활성화 (멈춘 세상 표현 용도)
	if(PlatformMesh)
	{
		PlatformMesh->SetSimulatePhysics(false);
	}
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGamePlatform::PlatformBoxOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AGamePlatform::PlatformBoxEndOverlap);
	
}

// Called every frame
void AGamePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//오버랩 체크 (디버깅 용)
void AGamePlatform::PlatformBoxOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult)
{
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherActor->IsA(AGameProjectile::StaticClass()))
    {
		UE_LOG(LogTemp, Display, TEXT("Overlap detected with %s"), *OtherActor->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Your message %f"), StoredImpulse.Size());
        // Handle overlap logic
    }

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherActor->IsA(AGameCharacter::StaticClass()))
    {
		AGameCharacter* Character = Cast<AGameCharacter>(OtherActor);

		UE_LOG(LogTemp, Display, TEXT("Overlap detected with %s"), *OtherActor->GetName());

		Overlapped = true;
		
		/*if (CharacterLocation.Z > PlatformLocation.Z)
		{
			if(!Character->IsAttachedTo(this))
			{
				Character->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
			}
			//Character->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		}*/
    }
}


//오버랩 탈출 확인 (디버깅 용)
void AGamePlatform::PlatformBoxEndOverlap(
	UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherActor->IsA(AGameCharacter::StaticClass()))
    {
		Overlapped = false;
    }
}


//누적된 충격량 부여 및 충격량 초기화화
void AGamePlatform::SetImpulse(FVector Impulse)
{
	PlatformMesh->AddImpulseAtLocation(Impulse, GetActorLocation());
	StoredImpulse = FVector::ZeroVector;
}

//충격량 누넉 저장
void AGamePlatform::StoreImpulse(FVector Impulse)
{
	StoredImpulse += Impulse;
}

//오버랩 여부 반환환
bool AGamePlatform::GetOverlapped()
{
	return Overlapped;
}