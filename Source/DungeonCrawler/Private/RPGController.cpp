// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGController.h"

#include "Blueprint/UserWidget.h"

void ARPGController::BeginPlay()
{
	Super::BeginPlay();
	if(IsLocalController())
	{
		if(StatBarUIClass && !StatBarWidget)
		{
			StatBarWidget = CreateWidget<UUserWidget>(this, StatBarUIClass);
			if(StatBarWidget)
			{
				StatBarWidget->AddToViewport();
			}
		}
	}
}

void ARPGController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	BindCurrentPawnCharacterToController();
}

void ARPGController::RespawnCharacter_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Respawning Character"));

	if(Lives > 0)
	{
		Lives--;
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(CharacterClass, GetPawn()->GetActorLocation(), FRotator(0,0,0), ActorSpawnParams);
		Possess(NewCharacter);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Game Over Man"));
	}
}

void ARPGController::UpdateCharacterStats(FCharacterStats CharacterStats)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Controller updated stats"));
	UpdateCharacterStatsBlueprintUI(CharacterStats);
}

void ARPGController::BindCurrentPawnCharacterToController()
{
	if(IsLocalController() && GetPawn())
	{
		ABaseCharacter* NewBaseCharacter = Cast<ABaseCharacter>(GetPawn());
		if(NewBaseCharacter && NewBaseCharacter != this->BaseCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Updated new base character reference."));
			this->BaseCharacter = NewBaseCharacter;
			NewBaseCharacter->OnUpdateCharacterStats.AddDynamic(this, &ARPGController::UpdateCharacterStats);
			NewBaseCharacter->ReceiveDeathEvent.AddDynamic(this, &ARPGController::RespawnCharacter);
			UpdateCharacterStats(NewBaseCharacter->GetCharacterStats());
		}
	}
}