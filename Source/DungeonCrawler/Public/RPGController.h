// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "RPGController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ARPGController : public APlayerController
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	int32 Lives = 3;
	virtual void BeginPlay() override;
	virtual void SetPawn(APawn* InPawn) override;
	UPROPERTY(BlueprintReadOnly)
	ABaseCharacter* BaseCharacter;
	// The class that will be used for the players Stat bar UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> HudWidgetClass;

	UPROPERTY(EditAnywhere, Category="Player")
	TSubclassOf<class ABaseCharacter> CharacterClass;

	// The instance of the players stat bar Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UUserWidget* HudWidget;
	
	UFUNCTION()
	void UpdateCharacterStats(FCharacterStats CharacterStats);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCharacterStatsBlueprintUI(FCharacterStats CharacterStats);

	void BindCurrentPawnCharacterToController();
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RespawnCharacter();
};
