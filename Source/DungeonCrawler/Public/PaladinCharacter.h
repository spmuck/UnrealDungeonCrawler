// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckHitboxCollision.h"
#include "RPGPlayerCharacter.h"
#include "PaladinCharacter.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API APaladinCharacter : public ARPGPlayerCharacter, public ICheckHitboxCollision
{
	GENERATED_BODY()
public:
	APaladinCharacter();
	bool ActorIsEnemy(AActor* OtherActor);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* MeleeHitbox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DashHitbox;

	UFUNCTION()
	void OnMeleeHitboxBeingOverlap(class UPrimitiveComponent* OverlappedComp,class AActor* OtherActor,class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	UFUNCTION()
	void ChangePrimaryHitboxCollisionType_Implementation(ECollisionEnabled::Type CollisionType) override;
	
};
