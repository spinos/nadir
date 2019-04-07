// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NadirActor.generated.h"

UCLASS()
class NADIR_API ANadirActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
