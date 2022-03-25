// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Curves/CurveVector.h"

// Sets default values
AItem::AItem():
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),
	// Item interp variables
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true),
	//Dynamic material parameters
	GlowAmount(150.f),
	FresnelExponent(3.f),
	FresnelRefractFraction(4.f),
	PulseCurveTime(5.f),
	SlotIndex(0),
	bCharacterInventoryFull(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Hide pickupWidget
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	// Sets active starts array based on item rarity
	SetActiveStars();

	// Setup overlap for Area Sphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Set Item properties based on item state
	SetItemProperties(ItemState);

	// Set custom depth to disabled
	InitializeCustomDepth();

	StartPulseTimer();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Handle item interping when in the EqipInterping state
	ItemInterp(DeltaTime);

	// Get Curve Values from pulse curve and set dynamic material parameters
	UpdatePulse();
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
			ShooterCharacter->UnHighlightInventorySlot();
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlapedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStars()
{
	// the zero element isnt used
	for (int32 i = 0; i<= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
		case EItemRarity::EIR_Damaged:
			ActiveStars[1] = true;
			break;
		case EItemRarity::EIR_Common:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			break;
		case EItemRarity::EIR_Uncommon:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			break;
		case EItemRarity::EIR_Rare:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			break;
		case EItemRarity::EIR_Legendery:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			ActiveStars[5] = true;
			break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
		case EItemState::EIS_Pickup:
			// Set Mesh properties
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Area Sphere properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			// Set Colliosion Box proprties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		case EItemState::EIS_Equipped:
			PickupWidget->SetVisibility(false);
			// Set Mesh properties
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Area Sphere properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Colliosion Box proprties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EItemState::EIS_Falling:
			// Set Mesh properties
			ItemMesh->SetSimulatePhysics(true);
			ItemMesh->SetEnableGravity(true);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			// Set Area Sphere properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Colliosion Box proprties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EItemState::EIS_EquipInterping:
			PickupWidget->SetVisibility(false);
			// Set Mesh properties
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Area Sphere properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Colliosion Box proprties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EItemState::EIS_Pickedup:
			PickupWidget->SetVisibility(false);
			// Set Mesh properties
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(false);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Area Sphere properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// Set Colliosion Box proprties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
	}
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound)
{
	// Store a handle to the charcter
	Character = Char;

	// Get array index in InterpLocations with the lowest item count
	InterpLocIndex = Character->GetInterpLocationIndex();
	// Add 1 to the item count for this interp location struct
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	PlayPickupSound(bForcePlaySound);
	// Store initial location of the item
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;

	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().ClearTimer(PulseTimer);

	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	// Get initial yaw of the camera
	const float CameraRotaionYaw {Character->GetFollowCamera()->GetComponentRotation().Yaw};
	// Get initial yaw of the item
	const float ItemRotationYaw {GetActorRotation().Yaw};
	// Initial Yaw offset between camera and item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotaionYaw;

	bCanChangeCustomDepth = false;
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		// Subtract 1 from the item count of the interp location struct
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
		Character->GetPickupItem(this);
		Character->UnHighlightInventorySlot();
	}
	// Set scale back to normal
	SetActorScale3D(FVector(1.f));

	DisableGlowMaterial();
	bCanChangeCustomDepth = true;
	DisableCustomDepth();
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (Character && ItemZCurve)
	{
		// Time that has passed since we started the timer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// Get curve value corresponding to elapsed time
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		// Get the items initial location when the curve started
		FVector ItemLocation = ItemInterpStartLocation;
		// Get Location in front of the camera
		const FVector CameraInterpLocation { GetInterpLocation() };

		// Vector from item to camera interp location X and Y are zeroed out
		const FVector ItemToCamera {FVector (0.f, 0.f, (CameraInterpLocation - ItemLocation).Z)};
		// Scale facotr to multiply with the curve value
		const float DeltaZ {ItemToCamera.Size()};

		const FVector CurrentLocation {GetActorLocation()};
		// Interplated X value
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.0f);
		// Interpolated Y value
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.0f);

		// Set X and Y of item location to interp values
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;
		
		// Adding curve value to the z component of the initial location (Scaled bu delta Z)
		ItemLocation.Z += CurveValue * DeltaZ;

		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);
		// Camera Rotattion this frame
		const FRotator CameraRotation {Character->GetFollowCamera()->GetComponentRotation()};
		// Camera rotation plus initial yaw offset
		FRotator ItemRotation {0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f};

		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);

			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector(0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;
	case EItemType::EIT_Weapon:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;
	}

	return FVector();
}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		else if (Character->ShouldPlayPickupSound())
		{
			Character->StartPickupSoundTimer();

			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}

		else if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();

			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
	}
	EnableGlowMaterial();
}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}	
}

void AItem::UpdatePulse()
{
	float ElapsedTime {};
	FVector CurveValue {};

	switch (ItemState)
	{	
		case EItemState::EIS_Pickup:
			if (PulseCurve)
			{
				ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
				CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
			}
			break;

		case EItemState::EIS_EquipInterping:
			if (InterpPulseCurve)
			{
				ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
				CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
			}
			break;
	}

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelRefractFraction);
	}
}