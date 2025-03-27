// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameDoor.generated.h"

UCLASS()
class GAME_API AGameDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetDoorState(bool state);

	UFUNCTION(BlueprintCallable)
	bool GetDoorState();

private:
	bool DoorState;

	private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Door, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorMesh;		

};
