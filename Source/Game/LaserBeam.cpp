// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserBeam.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

// Sets default values
ALaserBeam::ALaserBeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LaserBeamEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserBeamEffect"));
    LaserBeamEffect->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALaserBeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaserBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaserBeam::ActivateLaserBeam(const FVector& Start, const FVector& End)
{
    //무기 2번 기능 이펙트 활성화화
    if (LaserBeamEffect)
    {
        LaserBeamEffect->SetVectorParameter("Beam Start", Start);
        LaserBeamEffect->SetVectorParameter("Beam End", End);
        LaserBeamEffect->ActivateSystem();
    }
}