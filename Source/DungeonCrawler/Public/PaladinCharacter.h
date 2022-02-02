// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckHitboxCollision.h"
#include "RPGPlayerCharacter.h"
#include "Components/TimelineComponent.h"
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

	UFUNCTION()
	void DashTimelineProgress(float Value);

	UFUNCTION()
	void DashFinished();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* MeleeHitbox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DashHitbox;

	UPROPERTY(EditAnywhere, Category="Abilities")
	int32 DashDistance = 1000;

	UPROPERTY(EditAnywhere, Category="Abilities")
	int32 DashCost = 20;

	FTimeline DashTimeline;
	UPROPERTY(EditAnywhere, Category="Abilities")
	UCurveFloat* DashCurveFloat;

	UPROPERTY()
	FVector StartLocation;
	UPROPERTY()
	FVector EndLocation;
	
	UFUNCTION()
	void OnMeleeHitboxBeingOverlap(class UPrimitiveComponent* OverlappedComp,class AActor* OtherActor,class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDashHitboxBeginOverlap(class UPrimitiveComponent* OverlappedComp,class AActor* OtherActor,class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable, Category="Animation")
	void MultiDash();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//overrides implementation of server rpc
	virtual void ServerAbility1_Implementation() override;
public:
	UFUNCTION()
	void ChangePrimaryHitboxCollisionType_Implementation(ECollisionEnabled::Type CollisionType) override;
	
};
