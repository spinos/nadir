#include "NadirSceneComponent.h"

UNadirSceneComponent::UNadirSceneComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UNadirSceneComponent::PostLoad()
{
	Super::PostLoad();

	UE_LOG(LogTemp, Warning, TEXT("UNadirSceneComponent post load str %s"), *HierarchyStr );
}
