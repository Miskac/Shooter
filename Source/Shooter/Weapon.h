// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DispalayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DispalayName = "AssaultRifle"),

	EWT_MAX UMETA(DispalayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:

	AWeapon();

	virtual void Tick(float DeltaTime) override;

protected:

	void StopFalling();

private:

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	// Ammo count for this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Propertys", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	// The Type of weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Propertys", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	// The type of ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Propertys", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	// FName for the reload montage section
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Propertys", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

public:
	// Adds an impulse to the waepon
	void ThrowWeapon();

	FORCEINLINE int32 GetAmmo() const { return Ammo; };

	// Called from Character class when fireing weapon
	void DecrementAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }; 

	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; };

};
