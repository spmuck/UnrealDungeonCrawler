// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinCharacter.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

APaladinCharacter::APaladinCharacter()
{
	MeleeHitbox = CreateDefaultSubobject<UBoxComponent>(FName("MeleeHitbox"));
	MeleeHitbox->SetupAttachment(RootComponent);
	FVector RelativeLocation = FVector(140,0,0);
	MeleeHitbox->SetRelativeLocation(RelativeLocation);
	FVector RelativeScale = FVector(3.0,2.5,2.5);
	MeleeHitbox->SetRelativeScale3D(RelativeScale);
	MeleeHitbox->CanCharacterStepUpOn = ECB_No;
	//Assumes we have a profile named hitbox - should be no collision. Ignore all channels except overlap pawn and projectile.
	MeleeHitbox->SetCollisionProfileName(TEXT("Hitbox"));
	MeleeHitbox->SetGenerateOverlapEvents(true);

	DashHitbox = CreateDefaultSubobject<UBoxComponent>(FName("DashHitbox"));
	DashHitbox->SetupAttachment(RootComponent);
	RelativeLocation = FVector(65,0,0);
	DashHitbox->SetRelativeLocation(RelativeLocation);
	RelativeScale = FVector(2.25,2.5,2.5);
	DashHitbox->SetRelativeScale3D(RelativeScale);
	DashHitbox->CanCharacterStepUpOn = ECB_No;
	//Assumes we have a profile named hitbox - should be no collision. Ignore all channels except overlap pawn and projectile.
	DashHitbox->SetCollisionProfileName(TEXT("Hitbox"));
}

void APaladinCharacter::BeginPlay()
{
	//keep super call
	Super::BeginPlay();
	MeleeHitbox->OnComponentBeginOverlap.AddDynamic(this, &APaladinCharacter::OnMeleeHitboxBeingOverlap);
}

bool APaladinCharacter::ActorIsEnemy(AActor* OtherActor)
{
	return OtherActor->ActorHasTag(FName(TEXT("Enemy")));
}

void APaladinCharacter::OnMeleeHitboxBeingOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority() && ActorIsEnemy(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, 20.0f,GetController(), this, UDamageType::StaticClass());
	}
	
}



void APaladinCharacter::ChangePrimaryHitboxCollisionType_Implementation(ECollisionEnabled::Type CollisionType)
{
	MeleeHitbox->SetCollisionEnabled(CollisionType);
}

