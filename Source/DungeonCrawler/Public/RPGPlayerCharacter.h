// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "RPGPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ARPGPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ARPGPlayerCharacter();


protected:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void PrimaryAttack();
	void Blocking(bool IsBlocking);
	virtual void Ability1();
	virtual void Ability2();
	DECLARE_DELEGATE_OneParam(FBlockingDelegate, bool);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

public:
	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
