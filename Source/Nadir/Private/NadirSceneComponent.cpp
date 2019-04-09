#include "NadirSceneComponent.h"
#include "NadirUtil.h"

UNadirSceneComponent::UNadirSceneComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UNadirSceneComponent::encodeHierarchy(TSharedRef<FJsonObject> currentObj)
{
	currentObj->SetStringField("name", GetName());

	FTransform tm = GetRelativeTransform();
	FVector t = tm.GetTranslation();
	NadirUtil::EncodeTranslate(currentObj, t);
	FQuat r = tm.GetRotation();
	NadirUtil::EncodeRotate(currentObj, r);
	FVector s = tm.GetScale3D();
	NadirUtil::EncodeScale(currentObj, s);

	TArray < TSharedPtr < FJsonValue > > childArr;

	for (USceneComponent * achild : GetAttachChildren() ) {
		if (achild->IsA(UNadirSceneComponent::StaticClass())) {
			
			UNadirSceneComponent *ncomp = Cast<UNadirSceneComponent>(achild);

			TSharedRef<FJsonObject> childObj = MakeShared<FJsonObject>();
			ncomp->encodeHierarchy(childObj);

			childArr.Add(MakeShared<FJsonValueObject>(childObj));
		} 

		if (achild->IsA(UStaticMeshComponent::StaticClass())) {

			UStaticMeshComponent *msh = Cast<UStaticMeshComponent>(achild);

			TSharedRef<FJsonObject> childObj = MakeShared<FJsonObject>();
			
			NadirUtil::EncodeMeshComponent(childObj, msh);

			childArr.Add(MakeShared<FJsonValueObject>(childObj));
		}
	}

	currentObj->SetArrayField(TEXT("child"), childArr);
}
