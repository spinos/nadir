// Fill out your copyright notice in the Description page of Project Settings.

#include "NadirActor.h"
#include "NadirSceneComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ANadirActor::ANadirActor(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UNadirSceneComponent>("Root");
	
}

// Called when the game starts or when spawned
void ANadirActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANadirActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

