// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called for forwards/backwards input
	void MoveForward(float Value);

	// Called for right/left input
	void MoveRight(float Value);

	// Called via input to turn at a given rate
	// @param Rate This is a normlised rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	// Called via input to look up/down at a given rate
	// @param Rate This is a normlised rate, i.e. 1.0 means 100% of desired turn rate
	void LookUpAtRate(float Rate);

	// Rotate controlled based on mouse X movement
	// @param Value the input value from mouse movement
	void Turn(float Value);

	// Rotate controlled based on mouse Y movement
	// @param Value the input value from mouse movement
	void LookUp(float Value);

	// Called when the fire button is pressed
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	// Set bAiming to true of false with button press
	void AimingButtonPressed();

	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	// Set Base turn rate and Base look up rate based on aiming
	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();

	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	// Line trace for items under the hrosshairs
	bool TraceUnderCrosshairds(FHitResult& OutHitResult, FVector& OutHitLocation);

	// Trace for items if overlapped item count is greater then zero
	void TraceForItems();

	// Spawns default weapon and equips it
	class AWeapon* SpawnDefaultWeapon();

	// Takes a weapon and attaches it to the mesh
	void EquipWeapon(AWeapon* WeaponToEquip);

	// Detach weapon and let it fall to the ground
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	// Drops currently equipped weapon and equipps trace hit item
	void SwapWeapon(AWeapon* WeaponToSwap);

	// Initilize ammo map with ammo values
	void InitializeAmmoMap();

	// Chechk to make sure our weapon has ammo
	bool WeaponHasAmmo();
	// FireWeapon funtions
	void PlayFireSound();
	void SendBullet();
	void PlayGunfireMontage();
	// Bound to the r key and game pad face button left
	void ReloadButtonPressed();
	// Handle reloading of the weapon
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	// Checks to see if we have ammo of the Equipped Weapon weapon type
	bool CarryingAmmo();

	// Called from animation blueprint with grab clip notify
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	// Called from animation blueprint with release clip notify
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// CameraBoom is positioning camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Camera that follows the Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Base turn rate in deg/sec. Other scaling may affect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	// Base look up/down rate in deg/sec. Other scaling may affect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	// Turn rate while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	// Look up rate while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	// Turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	// Look up rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	// Scale factor for mouse look sensitivty. Turn rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	// Scale factor for mouse look sensitivty. Look up rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	// Scale factor for mouse look sensitivty. Turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	// Scale factor for mouse look sensitivty. Look up rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	// Randomised gunshot sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	// flash spawned at barrel socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	// Montage for fiering the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// Particles spawned upon bullet impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	// Smoke trail for bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// True when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// Defaunt camera field of view camera
	float CameraDefaultFOV;

	// Field of view value for when zoomed in
	float CameraZoomedFOV;

	// Current field of view this frame
	float CameraCurrentFOV;

	// Interp speed for zooming when aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// Determents the spred of the crosshairs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpredMultiplier;

	// Velocity component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	// In air component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	// Aim component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	// Shooting component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float ShootTimeDuration;

	bool bFiringBullet;

	FTimerHandle CrosshairShootTimer;

	// Left mouse button or right console trigger pressed
	bool bFireButtonPressed;

	// True when we can fire, flase when waiting for the timer
	bool bShouldFire;

	// Rate of automatic gun fire
	float AutomaticFireRate;

	// Sets a timer between gunshots
	FTimerHandle AutoFireTimer;

	// True if we should trace every frame for items
	bool bShouldTraceForItems;

	// Number of overlapped AItems
	int8 OverlappedItemCount;

	// The AItem we hit last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	// Currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	// Set this in Blueprints for the default weapon class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// The item currently hit by our trace in trace for items (could be null)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	// Distance outward from the camera for the interp desitnation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;
	// Distance upward from the camera fro the interp desitnation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	// Map to keep track of ammo of the different ammo types
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	// Strating amount of 9mm ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;
	// Strating amount of AR ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	// Combat state can only fire or reload if unoccupied
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	// Montage for Realod animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	// Transform of the clip when we first grab the clip during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	// Scene component to attach to the charctaers hand during relaoding
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	// True when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

public:
	// Returns CameraBoom Subobject
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; };
	// Returns FollowCamera Subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	FORCEINLINE bool GetAiming() const { return bAiming; };

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; };

	// Adds/subtracts to/from OverlappedItemCount and updates bShouldTraceForITems
	void IncrementOverlappedItemCount (int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; };

	FORCEINLINE bool GetCrouching () const { return bCrouching; };

};
