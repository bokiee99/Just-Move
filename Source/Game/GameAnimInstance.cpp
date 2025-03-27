// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAnimInstance.h"
#include "GameCharacter.h"

UGameAnimInstance::UGameAnimInstance()
{

}

void UGameAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if(GameCharacter == nullptr)
    {
        GameCharacter = Cast<AGameCharacter>(TryGetPawnOwner());
    }

    //캐릭터 앉기 애니메이션 여부 
    if(GameCharacter)
    {
        bCrouching = GameCharacter-> GetCrouching();
    }
}

void UGameAnimInstance::NativeInitializeAnimation()
{
    GameCharacter = Cast<AGameCharacter>(TryGetPawnOwner());
}   

void UGameAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    UpdateAnimationProperties(DeltaTime);
}
