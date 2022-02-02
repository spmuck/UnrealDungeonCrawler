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
	DashHitbox->OnComponentBeginOverlap.AddDynamic(this, &APaladinCharacter::OnDashHitboxBeginOverlap);

	if(DashCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineProgress.BindUFunction(this, FName("DashTimelineProgress"));
		TimelineFinishedCallback.BindUFunction(this, FName("DashFinished"));
		DashTimeline.AddInterpFloat(DashCurveFloat, TimelineProgress);
		DashTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		DashTimeline.SetLooping(false);
		DashTimeline.SetTimelineLengthMode(TL_TimelineLength);
		DashTimeline.SetTimelineLength(0.25);
	}
}

void APaladinCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	DashTimeline.TickTimeline(DeltaSeconds);
}

void APaladinCharacter::ServerAbility1_Implementation()
{
	if(CastIfEnoughMana(20))
	{
		MultiDash();
		bCurrentlyAttacking = true;
		MultiCharacterStatsChanged(CharacterStats);
	}
}

bool APaladinCharacter::ActorIsEnemy(AActor* OtherActor)
{
	return OtherActor->ActorHasTag(FName(TEXT("Enemy")));
}

void APaladinCharacter::DashTimelineProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Value);
	SetActorLocation(NewLocation);
}

void APaladinCharacter::DashFinished()
{
	bCurrentlyAttacking = false;
}

void APaladinCharacter::OnMeleeHitboxBeingOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority() && ActorIsEnemy(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, 20.0f,GetController(), this, UDamageType::StaticClass());
	}
	
}

void APaladinCharacter::MultiDash_Implementation()
{
	//set start location of dash and default end location if nothing is hit
	StartLocation = GetActorLocation();
	EndLocation  = GetActorLocation() + (DashDistance * GetActorForwardVector());

	//check for object in dash trace channel to see if we need to change EndLocation to be a valid location.
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if(GetWorld()->LineTraceSingleByChannel(Hit,StartLocation, EndLocation, ECC_GameTraceChannel1, QueryParams))
	{
		EndLocation = Hit.ImpactPoint;
		
	}
	//enable collisions to start checking for collisions
	DashHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//start dash timeline
	DashTimeline.PlayFromStart();
}


void APaladinCharacter::ChangePrimaryHitboxCollisionType_Implementation(ECollisionEnabled::Type CollisionType)
{
	MeleeHitbox->SetCollisionEnabled(CollisionType);
}

void APaladinCharacter::OnDashHitboxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority() && ActorIsEnemy(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, 100.0f,GetController(), this, UDamageType::StaticClass());
	}
}

