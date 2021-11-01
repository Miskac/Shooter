// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (ShooterCharacter == nullptr)
    {
        ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
    }

    if (ShooterCharacter)
    {
        // Get the latheral Speed of the character from velocity
        FVector Velocity {ShooterCharacter->GetVelocity()};
        Velocity.Z = 0;
        Speed = Velocity.Size();

        // Is the character in the air
        bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

        // Is the character moving
        if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
        {
            bIsAccelerating = true;
        }
        else
        {
            bIsAccelerating = false;
        }

        FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

        MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

        // FString RotationMessage = FString::Printf(TEXT("Base Aim Roation: %f"), AimRotation.Yaw);
        // FString MovementRotationMessage = FString::Printf(TEXT("Movement Roation: %f"), MovementRotation.Yaw);

        //FString OffsetMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffsetYaw);

        /*if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, OffsetMessage);
        }*/
    }
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
    ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

