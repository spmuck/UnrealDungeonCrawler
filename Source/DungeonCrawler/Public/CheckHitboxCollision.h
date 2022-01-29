// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CheckHitboxCollision.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCheckHitboxCollision : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUNGEONCRAWLER_API ICheckHitboxCollision
{
	GENERATED_BODY()

public:
	/**A version of React To Trigger that can be implemented in C++ or Blueprint. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Hitbox")
	void ChangePrimaryHitboxCollisionType(ECollisionEnabled::Type CollisionType);
};
