// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


ARPGPlayerCharacter::ARPGPlayerCharacter()
{
	Tags.Add(FName("Player"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->SetupAttachment(RootComponent);
	FRotator DefaultRotation = FRotator(-72.0, 0.0, 0.0);
	SpringArmComponent->SetWorldRotation(DefaultRotation);
	SpringArmComponent->TargetArmLength = 1250.0f;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ARPGPlayerCharacter::MoveForward(float AxisValue)
{
	if(IsMovementEnabled())
	{
		AddMovementInput(UKismetMathLibrary::GetForwardVector(GetControlRotation()), AxisValue);
	}
}

void ARPGPlayerCharacter::MoveRight(float AxisValue)
{
	if(IsMovementEnabled())
	{
		AddMovementInput(UKismetMathLibrary::GetRightVector(GetControlRotation()), AxisValue);
	}
}

void ARPGPlayerCharacter::PrimaryAttack()
{
	if(IsMovementEnabled())
	{
		ServerPrimaryAttack();
	}
}

void ARPGPlayerCharacter::Blocking(bool IsBlocking)
{
	ServerBlock(IsBlocking);
}

void ARPGPlayerCharacter::Ability1()
{
	ServerAbility1();
}

void ARPGPlayerCharacter::Ability2()
{
	ServerAbility2();
}

void ARPGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveForward", this,  &ARPGPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this,  &ARPGPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ARPGPlayerCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction<FBlockingDelegate>("Block", IE_Pressed, this, &ARPGPlayerCharacter::Blocking, true);
	PlayerInputComponent->BindAction<FBlockingDelegate>("Block", IE_Released, this, &ARPGPlayerCharacter::Blocking, false);
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &ARPGPlayerCharacter::Ability1);
	PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &ARPGPlayerCharacter::Ability2);
}
