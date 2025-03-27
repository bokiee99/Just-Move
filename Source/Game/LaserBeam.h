// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserBeam.generated.h"

UCLASS()
class GAME_API ALaserBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserBeam();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//무기 기능 2번 이펙트트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    class UNiagaraComponent* LaserBeamEffect;

	void ActivateLaserBeam(const FVector& Start, const FVector& End);

};
