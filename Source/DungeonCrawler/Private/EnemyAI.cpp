// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"

#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"

AEnemyAI::AEnemyAI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
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
	SetActorTickEnabled(false);
}

void AEnemyAI::SetCurrentlyAttacking(bool NewCurrentlyAttacking)
{
	Super::SetCurrentlyAttacking(NewCurrentlyAttacking);
	ChasePlayer();
}

void AEnemyAI::MultiKillCharacter()
{
	Super::MultiKillCharacter();
	PawnSensingComponent->Deactivate();
	CharacterTarget = nullptr;
	SetActorTickEnabled(false);
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

void AEnemyAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(CharacterTarget)
	{
		FRotator RotationToTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CharacterTarget->GetActorLocation());
		SetActorRotation(RotationToTarget);
	}
}

void AEnemyAI::OnPawnSeen(APawn* SeenPawn)
{
	if(!CharacterTarget && SeenPawn->ActorHasTag(FName("Player")) && !bDead)
	{
		ABaseCharacter* SeenCharacter = Cast<ABaseCharacter>(SeenPawn);
		if(SeenCharacter)
		{
			CharacterTarget = SeenCharacter;
			SetActorTickEnabled(true);
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
