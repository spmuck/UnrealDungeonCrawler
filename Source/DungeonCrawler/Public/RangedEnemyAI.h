// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAI.h"
#include "RangedEnemyAI.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ARangedEnemyAI : public AEnemyAI
{
	GENERATED_BODY()

public:
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void ShootProjectile();
	UPROPERTY(EditAnywhere, Category="Player")
	TSubclassOf<class AActor> ProjectileClass;
	
};
