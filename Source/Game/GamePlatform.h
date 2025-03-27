// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamePlatform.generated.h"

UCLASS()
class GAME_API AGamePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGamePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
    void PlatformBoxOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
    void PlatformBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
        AActor* OtherActor, 
        UPrimitiveComponent* OtherComp, 
        int32 OtherBodyIndex);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetImpulse(FVector Impulse);

	void StoreImpulse(FVector Impulse);

	bool GetOverlapped();

	FORCEINLINE FVector GetStoredImpulse() const {return StoredImpulse;}

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlatformMesh;		

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform, meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* CollisionBox;

	//현재 사용 안함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (AllowPrivateAccess = "true"))
    FVector StoredImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (AllowPrivateAccess = "true"))
    bool Overlapped;

	class AJustMoveProjectile* Projectile;

};
