// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"

#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"

AEnemyAI::AEnemyAI()
{
	Tags.Add(FName("Enemy"));
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	PawnSensingComponent->SightRadius = 1500.0f;
	PawnSensingComponent->SetPeripheralVisionAngle(180.0f);
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyAI::OnPawnSeen);

	GuardState = EAIState::Idle;
}

void AEnemyAI::MoveCompleted(FAIRequestID RquestID, EPathFollowingResult::Type Result)
{
	ServerPrimaryAttack();
}

void AEnemyAI::ResetCharacterTarget()
{
	CharacterTarget = nullptr;
}

void AEnemyAI::ResetCurrentlyAttacking()
{
	Super::ResetCurrentlyAttacking();
	ChasePlayer();
}

void AEnemyAI::ChasePlayer()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if(AIController && CharacterTarget)
	{
		AIController->MoveToActor(CharacterTarget, 600.0f);
		AIController->ReceiveMoveCompleted.RemoveAll(this);
		AIController->ReceiveMoveCompleted.AddDynamic(this,  &AEnemyAI::AEnemyAI::MoveCompleted);
	}
}

void AEnemyAI::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyAI, GuardState);
	DOREPLIFETIME(AEnemyAI, CharacterTarget);
}

void AEnemyAI::OnPawnSeen(APawn* SeenPawn)
{
	if(!CharacterTarget && SeenPawn->ActorHasTag(FName("Player")))
	{
		ABaseCharacter* SeenCharacter = Cast<ABaseCharacter>(SeenPawn);
		if(SeenCharacter)
		{
			CharacterTarget = SeenCharacter;
			SeenCharacter->ReceiveDeathEvent.AddDynamic(this, &AEnemyAI::ResetCharacterTarget);
			ChasePlayer();
		}
	}
	GuardState = EAIState::Alerted;
}

void AEnemyAI::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AEnemyAI::OnStateChanged(EAIState NewState)
{
}
