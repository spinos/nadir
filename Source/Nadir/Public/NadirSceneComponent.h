#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NadirSceneComponent.generated.h"

UCLASS()
class NADIR_API UNadirSceneComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:

	void encodeHierarchy(TSharedRef<FJsonObject> currentObj);

/// readonly in editor
///	UPROPERTY(VisibleAnywhere)
///	FString HierarchyStr;

private:

	void encodeMeshComponent(TSharedRef<FJsonObject> currentObj, UStaticMeshComponent *meshComp);

};
