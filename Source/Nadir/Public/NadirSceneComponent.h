#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NadirSceneComponent.generated.h"

UCLASS()
class NADIR_API UNadirSceneComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(VisibleInstanceOnly)
	FString HierarchyStr;

	virtual void PostLoad() override;
};
