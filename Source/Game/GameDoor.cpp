// Fill out your copyright notice in the Description page of Project Settings.


#include "GameDoor.h"
#include "Components/BoxComponent.h"


// Sets default values
AGameDoor::AGameDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorState = false;

	if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    }

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGameDoor::BeginPlay()
{
	Super::BeginPlay();

	//시작은 기본 비활성화 (멈춘 세상 표현)
	if(DoorMesh)
	{
		DoorMesh->SetSimulatePhysics(false);
	}
	
}

// Called every frame
void AGameDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//문 상태 체크 (열기, 닫기)
void AGameDoor::SetDoorState(bool state)
{
	DoorState = state;
}

//문 상태 반환
bool AGameDoor::GetDoorState()
{
	return DoorState;
}

