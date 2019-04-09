// Fill out your copyright notice in the Description page of Project Settings.

#include "NadirActor.h"
#include "NadirSceneComponent.h"
#include "NadirUtil.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonTypes.h"
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
	Root->encodeHierarchy(jobj);

	TSharedRef< TJsonWriter<> > jwriter = TJsonWriterFactory<>::Create(&hierarchyData.content);
	FJsonSerializer::Serialize(jobj, jwriter);
	jwriter->Close();

	UE_LOG(LogTemp, Warning, TEXT("ANadirActor pre save %s"), *hierarchyData.content);

}

void ANadirActor::PostLoad()
{
	Super::PostLoad();

	UE_LOG(LogTemp, Warning, TEXT("ANadirActor post load %s"), *hierarchyData.content);

	TSharedPtr<FJsonObject> jobj;
	TSharedRef<TJsonReader<> > jreader = TJsonReaderFactory<>::Create(hierarchyData.content);
	if (!FJsonSerializer::Deserialize(jreader, jobj)) return;
	if (!jobj.IsValid()) return;

	TArray<ComponentContentPair > nodes;
	nodes.Add(ComponentContentPair(Root, jobj) );

	while (nodes.Num() > 0) {

		ComponentContentPair &head = nodes[0];

		decodeHierarchy(head.Key, head.Value, nodes);

		nodes.RemoveAt(0);
	}
}

void ANadirActor::decodeHierarchy(USceneComponent *comp, const TSharedPtr<FJsonObject> &content, 
								TArray<ComponentContentPair > &nodes)
{
	FTransform tm = FTransform::Identity;
	bool tmChanged = false;

	FVector vt, vs;
	FQuat qr;

	if (NadirUtil::DecodeTranslate(vt, content)) {
		tm.SetLocation(vt);
		tmChanged = true;
	}

	if (NadirUtil::DecodeRotate(qr, content)) {
		tm.SetRotation(qr);
		tmChanged = true;
	}

	if (NadirUtil::DecodeScale(vs, content)) {
		tm.SetScale3D(vs);
		tmChanged = true;
	}

	if(tmChanged) comp->SetRelativeTransform(tm);

	const TArray<TSharedPtr<FJsonValue>>* childArr;
	if (!content->TryGetArrayField("child", childArr)) return;
	if (childArr->Num() < 1) return;

	for (TSharedPtr<FJsonValue> aval : *childArr) {
		const TSharedPtr < FJsonObject > & childObj = aval->AsObject();

		const FString childName = childObj->GetStringField("name");

		if (childObj->HasField("is_mesh")) {
			UStaticMeshComponent *meshComp = NewObject<UStaticMeshComponent>(this, FName(*childName));
			NadirUtil::DecodeMeshComponent(meshComp, childObj);
			attachNodeAddQueue(meshComp, comp, childObj, nodes);

/// somehow show the mesh
			meshComp->SetMobility(EComponentMobility::Static);
			
		}
		else {
			UNadirSceneComponent *childComp = NewObject<UNadirSceneComponent>(this, FName(*childName));
			attachNodeAddQueue(childComp, comp, childObj, nodes);
		}

	}
}

void ANadirActor::attachNodeAddQueue(USceneComponent *child, USceneComponent *parent, const TSharedPtr<FJsonObject> &content, 
							TArray<ComponentContentPair > &nodes)
{
	child->AttachToComponent(parent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	child->CreationMethod = EComponentCreationMethod::Instance;
	child->RegisterComponent();
	nodes.Add(ComponentContentPair(child, content));
}
