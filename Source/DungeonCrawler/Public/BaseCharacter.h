// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int32 Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int32 Mana = 100;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCharacterStatsSignature, FCharacterStats, CharacterStats);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlockingStateChangedSignature, bool, IsBlocking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPrimaryAttackSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlockedAttackSignature);

UCLASS()
class DUNGEONCRAWLER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	FCharacterStats CharacterStats;
	
	FName GetRandomSectionName(TArray<FName> SectionNames);

	UPROPERTY(Replicated, EditInstanceOnly, Category="Core")
	bool bDead = false;

	UPROPERTY(Replicated, EditInstanceOnly, Category="Core")
	bool bCurrentlyAttacking = false;

	UPROPERTY(Replicated, EditInstanceOnly, Category="Core")
	bool bCurrentlyBlocking = false;

	UPROPERTY(EditAnywhere, Category="Attack")
	float PrimaryAttackTime;

	FTimerHandle TimerHandle_CurrentlyAttackingTImer;

	UPROPERTY(EditAnywhere, Category="Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category="Attack")
	USoundBase* AttackSound;

	UPROPERTY(EditAnywhere, Category="Attack")
	TArray<FName> AttackMontageSections;

	bool IsMovementEnabled();

	UFUNCTION(Server, Reliable)
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser );

	bool IsDamageBlocked(AActor* DamageCauser);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsDead();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable, Category="Animation")
	void PlayAnimMontageAndSound(UAnimMontage* MontageToPlay, USoundBase* SoundBase, const FName& MontageSectionName);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable, Category="Animation")
	void BlockingStateChanged(bool IsBlocking);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Animation")
	void CharacterStatsChanged(FCharacterStats NewCharacterStats);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable, Category="Animation")
	void BlockedAttackFX();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void MultiKillCharacter();

	UFUNCTION(Server, Reliable)
	void ServerPrimaryAttack();

	UFUNCTION(Server, Reliable)
	virtual void ResetCurrentlyAttacking();
	
	UFUNCTION(Server, Reliable)
	void ServerBlock(bool IsBlocking);
	
	UFUNCTION(Server, Reliable)
	void ServerAbility1();

	UFUNCTION(Server, Reliable)
	void ServerAbility2();

	UPROPERTY(BlueprintAssignable)
	FUpdateCharacterStatsSignature OnUpdateCharacterStats;

	UPROPERTY(BlueprintAssignable)
	FDeathEventSignature ReceiveDeathEvent;

	UPROPERTY(BlueprintAssignable)
	FBlockingStateChangedSignature ReceiveBlockingStateChanged;

	UPROPERTY(BlueprintAssignable)
	FBlockedAttackSignature ReceiveBlockedAttackEvent;

	UPROPERTY(BlueprintAssignable)
	FPrimaryAttackSignature ReceivePrimaryAttack;

	UFUNCTION(BlueprintCallable)
	FCharacterStats GetCharacterStats();
};
