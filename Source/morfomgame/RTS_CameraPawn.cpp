// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_CameraPawn.h"

// Sets default values
ARTS_CameraPawn::ARTS_CameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARTS_CameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARTS_CameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARTS_CameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

