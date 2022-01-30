// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "CharacterAnimationBlueprint.h"
#include "RPGController.h"
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

void ABaseCharacter::OnRep_CharacterStats()
{
	//Won't execute in standalone mode, but needed for networked clients. 
	if(IsLocallyControlled())
	{
		OnUpdateCharacterStats.Broadcast(CharacterStats);
	}
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
		CharacterStats.Health -= Damage;
		if(CharacterStats.Health <= 0)
		{
			bDead = true;
			CharacterStats.Health = 0;
			MultiKillCharacter();
		}
	}
	//this is only run on server, so this wont execute in a networked situation, but is needed for standalone mode.
	if(IsLocallyControlled())
	{
		OnUpdateCharacterStats.Broadcast(CharacterStats);
	}
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

void ABaseCharacter::BlockingStateChanged_Implementation(bool IsBlocking)
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
	GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
	StopAnimMontage();
	ReceiveDeathEvent.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You dead"));
}

void ABaseCharacter::ServerAbility2_Implementation()
{
}

void ABaseCharacter::ServerAbility1_Implementation()
{
}

void ABaseCharacter::ServerBlock_Implementation(bool IsBlocking)
{
	if(!bCurrentlyAttacking && !bDead)
	{
		bCurrentlyBlocking = IsBlocking;
		BlockingStateChanged(IsBlocking);
	}
	else
	{
		bCurrentlyBlocking = false;
		BlockingStateChanged(false);
	}
}

void ABaseCharacter::ServerPrimaryAttack_Implementation()
{
	if (IsMovementEnabled())
	{
		bCurrentlyAttacking = true;
		GetWorldTimerManager().SetTimer(TimerHandle_CurrentlyAttackingTImer, this, &ABaseCharacter::ResetCurrentlyAttacking, PrimaryAttackTime, false);
		PlayAnimMontageAndSound(AttackMontage, AttackSound, GetRandomSectionName(AttackMontageSections));
	}
	
}

void ABaseCharacter::PlayAnimMontageAndSound_Implementation(UAnimMontage* MontageToPlay, USoundBase* SoundBase, const FName& MontageSectionName)
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
