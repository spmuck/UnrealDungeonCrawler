// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemyAI.generated.h"
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicious,
	Alerted
};
class UPawnSensingComponent;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API AEnemyAI : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemyAI();

protected:
	UPROPERTY(VisibleAnywhere, Category= "Components")
	UPawnSensingComponent* PawnSensingComponent;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;

	UFUNCTION()
	void OnRep_GuardState();
	
	UFUNCTION()
	void OnStateChanged(EAIState NewState);

	UPROPERTY(Replicated, VisibleAnywhere, Category="AI")
	ABaseCharacter* CharacterTarget;

	UFUNCTION()
	void MoveCompleted(FAIRequestID RquestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	void ResetCharacterTarget();
public:
	virtual void ResetCurrentlyAttacking() override;
	virtual void MultiKillCharacter() override;

	UFUNCTION()
	void ChasePlayer();
};
