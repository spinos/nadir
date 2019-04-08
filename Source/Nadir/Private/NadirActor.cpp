// Fill out your copyright notice in the Description page of Project Settings.

#include "NadirActor.h"
#include "NadirSceneComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonTypes.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

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

void ANadirActor::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);

	TSharedRef<FJsonObject> jobj(new FJsonObject());
	jobj->SetStringField("Name", "Super Sword");
	jobj->SetNumberField("Damage", 15);
	jobj->SetNumberField("Weight", 3);

	TSharedRef<FJsonObject> child1 = MakeShared<FJsonObject>();
	child1->SetStringField("Name", "Fire");
	child1->SetNumberField("Damage", 25);
	child1->SetNumberField("Weight", 1);

	TSharedRef<FJsonObject> child2 = MakeShared<FJsonObject>();
	child2->SetStringField("Name", "Water");
	child2->SetNumberField("Damage", 22);
	child2->SetNumberField("Weight", 2);

	TArray < TSharedPtr < FJsonValue > > childArr;
	childArr.Add(MakeShared<FJsonValueObject>(child1) );
	childArr.Add(MakeShared<FJsonValueObject>(child2));

	jobj->SetArrayField(TEXT("child"), childArr);

	TSharedRef< TJsonWriter<> > jwriter = TJsonWriterFactory<>::Create(&hierarchyData.content);
	FJsonSerializer::Serialize(jobj, jwriter);
	jwriter->Close();

	UE_LOG(LogTemp, Warning, TEXT("ANadirActor pre save %s"), *hierarchyData.content);

}

void ANadirActor::PostLoad()
{
	Super::PostLoad();

	UE_LOG(LogTemp, Warning, TEXT("ANadirActor post load %s"), *hierarchyData.content);
}
