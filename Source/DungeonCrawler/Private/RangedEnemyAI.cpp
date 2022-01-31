// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedEnemyAI.h"

#include "Kismet/KismetMathLibrary.h"

void ARangedEnemyAI::ShootProjectile_Implementation()
{
	if (CharacterTarget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Shooting my load lmao."));
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParams.Owner = this;
		ActorSpawnParams.Instigator = this;
		GetWorld()->SpawnActor<AActor>(ProjectileClass, GetActorLocation() + (GetActorForwardVector() * 40) , GetActorRotation(),ActorSpawnParams);
	}
}
