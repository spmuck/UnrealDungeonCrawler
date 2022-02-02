// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::ABaseCharacter::TakeAnyDamage);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

FName ABaseCharacter::GetRandomSectionName(TArray<FName> SectionNames)
{
	if(SectionNames.Num() > 0)
	{
		FName SectionName = SectionNames[FMath::RandRange(0, AttackMontageSections.Num() - 1)];
		return SectionName;
	}
	else
	{
		return FName(TEXT("Default"));
	}
}

bool ABaseCharacter::IsMovementEnabled()
{
	return !bCurrentlyAttacking && !bDead && !bCurrentlyBlocking;
}

void ABaseCharacter::TakeAnyDamage_Implementation(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Received %f damage"),Damage));
	if (!bDead)
	{
		if (!IsDamageBlocked(DamageCauser))
		{
			CharacterStats.Health -= Damage;
			if(CharacterStats.Health <= 0)
			{
				bDead = true;
				CharacterStats.Health = 0;
				MultiKillCharacter();
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Blocked same damage"));
			MultiBlockedAttackFX();
		}
	}
	MultiCharacterStatsChanged(CharacterStats);
}

void ABaseCharacter::MultiBlockedAttackFX_Implementation()
{
	ReceiveBlockedAttackEvent.Broadcast();
}

bool ABaseCharacter::IsDamageBlocked(AActor* DamageCauser)
{
	//If we are not currently blocking, don't run block direction check
	if(!bCurrentlyBlocking) return false;
	//Check if damage causer is infront of the current actor.
	FVector BlockingActorXYForwardVector = FVector(GetActorForwardVector().X, GetActorForwardVector().Y,0.0);
	BlockingActorXYForwardVector.Normalize(0.0001);
	FVector DirectionFromDamageCauserToDamagedActor = GetActorLocation() - DamageCauser->GetActorLocation();
	FVector DamageCauserToBlockingActorXYVector = FVector(DirectionFromDamageCauserToDamagedActor.X,DirectionFromDamageCauserToDamagedActor.Y, 0.0);
	DamageCauserToBlockingActorXYVector.Normalize(0.0001);
	float dotProduct = FVector::DotProduct(BlockingActorXYForwardVector, DamageCauserToBlockingActorXYVector);
	bool IsInFrontOf =  dotProduct <= -0.4;
	return IsInFrontOf;
}

//call when casting spells to subtract mana cost if character has enough mana. Return true if mana cost was payed or false if it was unable to be paid.
//only call from authority (server if in multiplayer mode)
bool ABaseCharacter::CastIfEnoughMana(int32 ManaCost)
{
	if(HasAuthority() && (CharacterStats.Mana - ManaCost >=0))
	{
		CharacterStats.Mana -= ManaCost;
		return true;
	}
	return false;
}


// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABaseCharacter::IsDead()
{
	return bDead;
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::MultiCharacterStatsChanged_Implementation(FCharacterStats NewCharacterStats)
{
	OnUpdateCharacterStats.Broadcast(NewCharacterStats);
}

void ABaseCharacter::MultiBlockingStateChanged_Implementation(bool IsBlocking)
{
	ReceiveBlockingStateChanged.Broadcast(IsBlocking);
}

void ABaseCharacter::ResetCurrentlyAttacking_Implementation()
{
	bCurrentlyAttacking = false;
}

FCharacterStats ABaseCharacter::GetCharacterStats()
{
	return CharacterStats;
}

void ABaseCharacter::MultiKillCharacter_Implementation()
{
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetAvoidanceEnabled(false);
	GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
	SetActorTickEnabled(false);
	StopAnimMontage();
	ReceiveDeathEvent.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You dead"));
}

void ABaseCharacter::ServerAbility2_Implementation()
{
}

void ABaseCharacter::ServerAbility1_Implementation()
{
	if(CastIfEnoughMana(20))
	{
		
	}
	MultiCharacterStatsChanged(CharacterStats);
}

void ABaseCharacter::ServerBlock_Implementation(bool IsBlocking)
{
	if(!bCurrentlyAttacking && !bDead)
	{
		bCurrentlyBlocking = IsBlocking;
		MultiBlockingStateChanged(IsBlocking);
	}
	else
	{
		bCurrentlyBlocking = false;
		MultiBlockingStateChanged(false);
	}
}

void ABaseCharacter::ServerPrimaryAttack_Implementation()
{
	if (IsMovementEnabled())
	{
		bCurrentlyAttacking = true;
		GetWorldTimerManager().SetTimer(TimerHandle_CurrentlyAttackingTImer, this, &ABaseCharacter::ResetCurrentlyAttacking, PrimaryAttackTime, false);
		MultiPlayAnimMontageAndSound(AttackMontage, AttackSound, GetRandomSectionName(AttackMontageSections));
	}
	
}

void ABaseCharacter::MultiPlayAnimMontageAndSound_Implementation(UAnimMontage* MontageToPlay, USoundBase* SoundBase, const FName& MontageSectionName)
{
	//ReceivePrimaryAttack.Broadcast();
	PlayAnimMontage(MontageToPlay, 1.0f, MontageSectionName);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundBase, GetActorLocation());
}

void ABaseCharacter::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( ABaseCharacter, bCurrentlyAttacking );
	DOREPLIFETIME( ABaseCharacter, bDead );
	DOREPLIFETIME( ABaseCharacter, bCurrentlyBlocking );
	DOREPLIFETIME( ABaseCharacter, CharacterStats );
}
