/*
 *  NadirUtil.cpp
 *
 *  https://answers.unrealengine.com/questions/813601/view.html
 *  Plugins/MovieScene/LevelSequenceEditor/Source/LevelSequenceEditor/Private/LevelSequenceEditorToolkit.cpp
 * 
 *  2019/5/30
 */

#include "NadirUtil.h"
#include "AssetRegistryModule.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogNadirUtil);


/// https://docs.unrealengine.com/en-us/Engine/Sequencer/Workflow/SequencerTimeRefactorNotes
int32 NadirUtil::CalcFrameNumber(int32 fframe, int32 fpsUp, int32 tickUp)
{
	return fframe * fpsUp / tickUp;
}

ULevelSequence* NadirUtil::GetActiveLevelSequence()
{
	TArray < UObject * > currentAssets = FAssetEditorManager::Get().GetAllEditedAssets();
	if (currentAssets.Num() < 1) {
		UE_LOG(LogNadirUtil, Warning, TEXT("no active asset"));
		return nullptr;
	}

	ULevelSequence* levelSeq = Cast<ULevelSequence>(currentAssets[0]);
	if (!levelSeq)
	{
		UE_LOG(LogNadirUtil, Warning, TEXT("no active level sequence"));
		return nullptr;
	}

	return levelSeq;
}

AActor *NadirUtil::GetFirstSelectedActorName(FString &actorName)
{
	/// 	int32 numSelActors = GEditor->GetSelectedActorCount();
	/// 	UE_LOG(LogNadirUtil, Warning, TEXT("# Selected Actor %i"), numSelActors );

	USelection* selActors = GEditor->GetSelectedActors();
	for (FSelectionIterator Iter(*selActors); Iter; ++Iter)
	{
		if (AActor* levelActor = Cast<AActor>(*Iter))
		{
			actorName = levelActor->GetName();
			/// type of actor
			if (levelActor->IsA(ACameraActor::StaticClass())) {
				UE_LOG(LogNadirUtil, Log, TEXT("%s is a camera "), *actorName);
			}
			return levelActor;
		}
	}

	UE_LOG(LogNadirUtil, Error, TEXT("nothing selected"));
	return nullptr;
}

UMovieScene3DTransformTrack *NadirUtil::GetActorTransformTrack(ULevelSequence* levelSeq, const FString &actorName)
{
	UMovieScene *movScn = levelSeq->GetMovieScene();

	const TArray < FMovieSceneBinding > &movBinds = movScn->GetBindings();
	///		UE_LOG(LogNadirUtil, Warning, TEXT("# bindings %i "), movBinds.Num() );

	for (int i = 0; i < movBinds.Num(); i++) {

		///			const TArray < UMovieSceneTrack * > & tracks = movBinds[i].GetTracks();

		const FGuid& objId = movBinds[i].GetObjectGuid();
		static const FName s_transformTrackName(TEXT("Transform"));

		UMovieScene3DTransformTrack *transformTrack = movScn->FindTrack<UMovieScene3DTransformTrack>(objId, s_transformTrackName);
		if (!transformTrack) {
			continue;
		}

		if (transformTrack->IsEmpty()) {
			UE_LOG(LogNadirUtil, Error, TEXT("empty transform track"));
			continue;
		}

		TArray < UObject *, TInlineAllocator < 1 > > boundObjs;
		/// connection between binding id and actor reference
		levelSeq->LocateBoundObjects(objId, nullptr, boundObjs);

		if (boundObjs.Num() < 1) {
			continue;
		}

		TWeakObjectPtr<UObject> firstBoundObjPtr = boundObjs[0];
		UObject *firstBoundObj = firstBoundObjPtr.Get();
		AActor *firstActor = Cast<AActor>(firstBoundObj);

		if (!firstActor) {
			continue;
		}

		if (firstActor->GetName() == actorName) {
			return transformTrack;
		}

	}

	UE_LOG(LogNadirUtil, Error, TEXT("cannot find transform track for %s"), *actorName);
	return nullptr;
}

/// https://answers.unrealengine.com/questions/536269/add-sequencer-tracks-from-code.html
void NadirUtil::CreateActorTransformTrack(ULevelSequence* levelSeq, const FString &actorName)
{
	UE_LOG(LogNadirUtil, Log, TEXT("CreateActorTransformTrack %s "), *actorName );

	UMovieScene *movScn = levelSeq->GetMovieScene();
	const TArray < FMovieSceneBinding > &movBinds = movScn->GetBindings();
	for (int i = 0; i < movBinds.Num(); i++) {
		const FGuid& objId = movBinds[i].GetObjectGuid();

		TArray < UObject *, TInlineAllocator < 1 > > boundObjs;
		levelSeq->LocateBoundObjects(objId, nullptr, boundObjs);

		if (boundObjs.Num() < 1) {
			continue;
		}

		TWeakObjectPtr<UObject> firstBoundObjPtr = boundObjs[0];
		UObject *firstBoundObj = firstBoundObjPtr.Get();
		AActor *firstActor = Cast<AActor>(firstBoundObj);

		if (!firstActor) {
			continue;
		}

		if (firstActor->GetName() != actorName) {
			continue;
		}

		static const FName s_transformTrackName(TEXT("Transform"));
		UMovieScene3DTransformTrack *transformTrack = movScn->FindTrack<UMovieScene3DTransformTrack>(objId, s_transformTrackName);
		if (!transformTrack) {
			transformTrack = Cast<UMovieScene3DTransformTrack>(movScn->AddTrack(UMovieScene3DTransformTrack::StaticClass(), objId));
		}

		if (transformTrack->IsEmpty()) {
			UE_LOG(LogNadirUtil, Warning, TEXT("add a section to empty transform track reload to view it"));
			UMovieScene3DTransformSection *sec = Cast<UMovieScene3DTransformSection>(transformTrack->CreateNewSection());
			transformTrack->AddSection(*sec);
		}

		UMovieScene3DTransformSection *firstSection = Cast<UMovieScene3DTransformSection>(transformTrack->GetAllSections()[0]);
		KeyTransformTrackSection(firstSection);

	}
}

void NadirUtil::CountLevelSequences()
{
	FAssetRegistryModule* assetRegModule = FModuleManager::LoadModulePtr<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	if (!assetRegModule)
	{
		UE_LOG(LogNadirUtil, Warning, TEXT("AssetRegistry module not loaded"));
		return;
	}
	//UE_LOG(LogNadirUtil, Warning, TEXT("AssetRegistry memory usage = %.2f MB"), assetRegModule->Get().GetAllocatedSize() / (1024.0f * 1024.0f));

	IAssetRegistry& assetRegistry = assetRegModule->Get();
	TArray<FAssetData> levelSequenceAssetList;
	assetRegistry.GetAssetsByClass(ULevelSequence::StaticClass()->GetFName(), levelSequenceAssetList);

	UE_LOG(LogNadirUtil, Log, TEXT("# level sequence asset %i "), levelSequenceAssetList.Num());

	//for(FAssetData levelSeqAsset : levelSequenceAssetList) 
	//{
	//	ULevelSequence *levelSeq = Cast<ULevelSequence>(levelSeqAsset.GetAsset());
		/// level sequence editor
		//IAssetEditorInstance* assetEditor = FAssetEditorManager::Get().FindEditorForAsset(levelSeq, true);
	//}

}

void NadirUtil::CheckPossessed(ULevelSequence* levelSeq)
{
	UMovieScene *movScn = levelSeq->GetMovieScene();

	UE_LOG(LogNadirUtil, Warning, TEXT("# possessables %i "), movScn->GetPossessableCount());

	for (int i = 0; i < movScn->GetPossessableCount(); i++) {
		FMovieScenePossessable & possi = movScn->GetPossessable(i);
		UE_LOG(LogNadirUtil, Warning, TEXT("possessable %s "), *possi.GetName());

		const FGuid & possId = possi.GetGuid();
		UE_LOG(LogNadirUtil, Warning, TEXT("parent id is %i %i %i %i"), possId.A, possId.B, possId.C, possId.D);

	}

}

bool NadirUtil::CheckTrackSections(const TArray < UMovieSceneSection * > & trackSecs)
{
	if(trackSecs.Num() < 1) return false;

	for(UMovieSceneSection * asect : trackSecs) {
		TRange < FFrameNumber > sectRange = asect->GetRange();
		FMovieSceneChannelProxy & channelPrx = asect->GetChannelProxy();

		LsChannels(channelPrx);

	}

	return true;
}

void NadirUtil::LsChannels(FMovieSceneChannelProxy & channelPrx)
{
	TArrayView < const FMovieSceneChannelMetaData > channelData = channelPrx.GetMetaData<FMovieSceneFloatChannel>();
	TArrayView<FMovieSceneFloatChannel *> channels = channelPrx.GetChannels<FMovieSceneFloatChannel>();
		
	for(const FMovieSceneChannelMetaData adata : channelData) {
		FName channelName = adata.Name;
		uint8 channelOrder = adata.SortOrder;

		UE_LOG(LogNadirUtil, Warning, TEXT("found channel %s order %i "), *channelName.ToString(), channelOrder );

		const FMovieSceneFloatChannel *achannel = channels[channelOrder];

		LsKeys(*achannel);

	}
}

void NadirUtil::LsKeys(const FMovieSceneFloatChannel &achannel)
{
	UE_LOG(LogNadirUtil, Warning, TEXT("# key %i "), achannel.GetNumKeys());

	TArrayView<const FFrameNumber>          channelTimes  = achannel.GetTimes();
	TArrayView<const FMovieSceneFloatValue> channelValues = achannel.GetValues();

	for (int32 i = 0; i < channelTimes.Num(); ++i) {
		FFrameNumber          keyTime  = channelTimes[i];
		FMovieSceneFloatValue keyValue = channelValues[i];

		UE_LOG(LogNadirUtil, Log, TEXT("key time %i value %f interp mode %s tangent mode %s"), 
			keyTime.Value, keyValue.Value,
			*InterpModeAsStr(keyValue.InterpMode), *TangentModeAsStr(keyValue.TangentMode));

		if(keyValue.InterpMode = ERichCurveInterpMode::RCIM_Cubic) PrintTangent(keyValue.Tangent);
	}
}

FString NadirUtil::InterpModeAsStr(ERichCurveInterpMode x)
{
	if(x == ERichCurveInterpMode::RCIM_Cubic)
		return FString("cubic");

	if(x == ERichCurveInterpMode::RCIM_Linear)
		return FString("linear");

	if(x == ERichCurveInterpMode::RCIM_Constant)
		return FString("constant");

	return FString("none");
}

FString NadirUtil::TangentModeAsStr(ERichCurveTangentMode x)
{
	if(x == ERichCurveTangentMode::RCTM_Auto)
		return FString("auto");

	if(x == ERichCurveTangentMode::RCTM_User)
		return FString("user");

	if(x == ERichCurveTangentMode::RCTM_Break)
		return FString("break");

	return FString("none");
}

void NadirUtil::PrintTangent(const FMovieSceneTangentData & tangent)
{
	FString twm("none");

	if(tangent.TangentWeightMode == ERichCurveTangentWeightMode::RCTWM_WeightedArrive)
		twm = FString("arrive");

	else if(tangent.TangentWeightMode == ERichCurveTangentWeightMode::RCTWM_WeightedLeave)
		twm = FString("leave");

	else if(tangent.TangentWeightMode == ERichCurveTangentWeightMode::RCTWM_WeightedBoth)
		twm = FString("both");

	UE_LOG(LogNadirUtil, Log, TEXT("arrive %f weight %f leave %f weight %f mode %s"), 
		tangent.ArriveTangent,
		tangent.ArriveTangentWeight,
		tangent.LeaveTangent,
		tangent.LeaveTangentWeight,
		*twm
	);

/// Runtime/Core/Public/Misc/FrameRate.h
/// AsInterval() = double(Denominator) / double(Numerator) "The time in seconds for a single frame under this frame rate"
/// AsDecimal() = double(Numerator) / double(Denominator)
/// Runtime/MovieScene/Private/Channels/MovieSceneFloatChannel.cpp
/// const float PrevToNextTimeDiff = FMath::Max<double>(KINDA_SMALL_NUMBER, Times[Index+1].Value - Times[Index-1].Value);
/// AutoCalcTangent(PrevKey.Value, ThisKey.Value, NextKey.Value, Tension, NewTangent);
/// NewTangent /= PrevToNextTimeDiff;
/// Runtime/Core/Public/Math/InterpCurvePoint.h
/// OutTan = (1.f - Tension) * ( (P - PrevP) + (NextP - P) ) where Tension is 0
/// angle = atan(dQ/dt)
/// dt in ticks
/// Q changed 1 in 1 second, angle is 45 deg
/// tangent = angle / ticks-per-second
/// tangent is dQ-per-tick
	float ticksPerSecond = 24000;
	float tangentAngle = FMath::Atan(tangent.ArriveTangent * ticksPerSecond);
	float tangentAngleInDegree = tangentAngle * 180 / 3.14159;

/// TickResolution        The tick resolution with which to interpret this channel's times
/// const float TimeInterval = TickResolution.AsInterval();
///	const float ToSeconds = 1.0f / TimeInterval;
/// const double Time1 = TickResolution.AsSeconds(Times[Index1].Value);
///	const double Time2 = TickResolution.AsSeconds(Times[Index2].Value);
///	const float X = Time2 - Time1;
/// float Angle = FMath::Atan(Key1.Tangent.LeaveTangent * ToSeconds);
///	const float ArriveTangentNormalized = Key2.Tangent.ArriveTangent / (TimeInterval);
///	const float Y = ArriveTangentNormalized * X;
///	ArriveWeight = FMath::Sqrt(X*X + Y * Y) * OneThird;
/// weight is base on second

	float oneThird = 1.f / 3.f;	
	float secondPerTick = 1.f / 24000.f;
	
/// normalized to change-per-second
	const float tangentNormalized = tangent.ArriveTangent * ticksPerSecond;
/// 1 second
	float dt = 1.f;
	float dQ = tangentNormalized * dt;
/// default (none) weight has nothing to do with weight displayed
/// simply the lenght of hypotenuse 
	float tangentWeight = FMath::Sqrt(dQ * dQ + dt * dt) * oneThird;
/// user weight is absolute y size at the end of the handle? 
/// independent of dQ and dt

/// unweighted bezier curve control points depends on time difference
///	const int32 Diff = Times[Index2].Value - Times[Index1].Value;
///	const float P0 = Key1.Value;
///	const float P1 = P0 + (Key1.Tangent.LeaveTangent * Diff * OneThird);
///	const float P3 = Key2.Value;
///	const float P2 = P3 - (Key2.Tangent.ArriveTangent * Diff * OneThird);
/// BezierInterp(P0, P1, P2, P3, Interp); where Interp is t
///	OutValue = Params.ValueOffset + BezierInterp(P0, P1, P2, P3, Interp);

	UE_LOG(LogNadirUtil, Error, TEXT("arrive tangent angle %f weight %f"),
		tangentAngleInDegree,
		tangentWeight
	);
}

void NadirUtil::GetActorTypeName(FString &typeName, const AActor *actor)
{
	if (actor->IsA(ACameraActor::StaticClass())) {
		typeName = FString("camera");
	}
}

UWorld* NadirUtil::GetEditorWorld()
{
	for (const FWorldContext& context : GEngine->GetWorldContexts())
	{
		if (context.WorldType == EWorldType::Editor)
		{
			return context.World();
		}
	}

	return nullptr;
}

UStaticMesh* NadirUtil::CreateMinimalMesh()
{
	FRawMesh raw;
	TArray < FVector > &vs = raw.VertexPositions;

	vs.Add(FVector(0.f, 0.f, 3.f));
	vs.Add(FVector(100.f, 0.f, 3.f));
	vs.Add(FVector(100.f, 100.f, 3.f));
	vs.Add(FVector(0.f, 100.f, 3.f));

	TArray < uint32 > &inds = raw.WedgeIndices;
	/// clockwise
	inds.Add(0);
	inds.Add(2);
	inds.Add(1);

	inds.Add(0);
	inds.Add(3);
	inds.Add(2);

	FVector tx(1.0, 0.0, 0.0);
	FVector ty(0.0, 1.0, 0.0);
	FVector tz(0.0, 0.0, 1.0);

	TArray < FVector > &wedgeTx = raw.WedgeTangentX;
	TArray < FVector > &wedgeTy = raw.WedgeTangentY;
	TArray < FVector > &wedgeTz = raw.WedgeTangentZ;

	wedgeTx.Add(tx);
	wedgeTy.Add(ty);
	wedgeTz.Add(tz);

	wedgeTx.Add(tx);
	wedgeTy.Add(ty);
	wedgeTz.Add(tz);

	wedgeTx.Add(tx);
	wedgeTy.Add(ty);
	wedgeTz.Add(tz);

	wedgeTx.Add(tx);
	wedgeTy.Add(ty);
	wedgeTz.Add(tz);

	wedgeTx.Add(tx);
	wedgeTy.Add(ty);
	wedgeTz.Add(tz);

	wedgeTx.Add(tx);
	wedgeTy.Add(ty);
	wedgeTz.Add(tz);

	/// at least one uv set
	TArray < FVector2D > &uv = raw.WedgeTexCoords[0];

	uv.Add(FVector2D(0.f, 0.f));
	uv.Add(FVector2D(1.f, 1.f));
	uv.Add(FVector2D(1.f, 0.f));

	uv.Add(FVector2D(0.f, 0.f));
	uv.Add(FVector2D(0.f, 1.f));
	uv.Add(FVector2D(1.f, 1.f));

	TArray < int32 > &faceMat = raw.FaceMaterialIndices;
	TArray < uint32 > &smoothMsk = raw.FaceSmoothingMasks;

	faceMat.Add(0);
	smoothMsk.Add(0);

	faceMat.Add(0);
	smoothMsk.Add(0);

	UE_LOG(LogNadirUtil, Warning, TEXT("raw nv %i ni %i valid %i"), raw.VertexPositions.Num(), raw.WedgeIndices.Num(),
		raw.IsValid());

	/// Make sure rendering is done
	FlushRenderingCommands();

	FString packageName("/Game/Meshes/myPackage");

	UPackage* package = CreatePackage(NULL, *packageName);
	if (!package)
		UE_LOG(LogNadirUtil, Warning, TEXT("create package error %s "), *packageName);

	package->FullyLoad();

	FString meshName("myMesh");

	UStaticMesh* mesh = NewObject<UStaticMesh>(package, FName(*meshName), RF_Public | RF_Standalone);
	if (!mesh)
		UE_LOG(LogNadirUtil, Warning, TEXT("create mesh error %s "), *meshName);

	mesh->InitResources();

	FStaticMeshSourceModel& srcM = mesh->AddSourceModel();
	srcM.SaveRawMesh(raw);

	mesh->StaticMaterials.Add(FStaticMaterial());

	mesh->Build(false);
	mesh->PostEditChange();

	/// Notify asset registry of new asset
	FAssetRegistryModule::AssetCreated(mesh);
	/// to be saved
	package->MarkPackageDirty();

	return mesh;
}

void NadirUtil::EncodeTranslate(TSharedRef<FJsonObject> obj, const FVector &t)
{
	if (t.IsZero()) return;

	TSharedPtr < FJsonValueNumber > tx = MakeShared<FJsonValueNumber>(t.X);
	TSharedPtr < FJsonValueNumber > ty = MakeShared<FJsonValueNumber>(t.Y);
	TSharedPtr < FJsonValueNumber > tz = MakeShared<FJsonValueNumber>(t.Z);

	TArray < TSharedPtr < FJsonValue > > tArr;
	tArr.Add(tx);
	tArr.Add(ty);
	tArr.Add(tz);

	obj->SetArrayField("t", tArr);
}

void NadirUtil::EncodeRotate(TSharedRef<FJsonObject> obj, const FQuat &q)
{
	if (q == FQuat::Identity) return;

	TSharedPtr < FJsonValueNumber > qx = MakeShared<FJsonValueNumber>(q.X);
	TSharedPtr < FJsonValueNumber > qy = MakeShared<FJsonValueNumber>(q.Y);
	TSharedPtr < FJsonValueNumber > qz = MakeShared<FJsonValueNumber>(q.Z);
	TSharedPtr < FJsonValueNumber > qw = MakeShared<FJsonValueNumber>(q.W);

	TArray < TSharedPtr < FJsonValue > > qArr;
	qArr.Add(qx);
	qArr.Add(qy);
	qArr.Add(qz);
	qArr.Add(qw);

	obj->SetArrayField("r", qArr);
}

void NadirUtil::EncodeScale(TSharedRef<FJsonObject> obj, const FVector &s)
{
	if (s == FVector::OneVector) return;

	TSharedPtr < FJsonValueNumber > sx = MakeShared<FJsonValueNumber>(s.X);
	TSharedPtr < FJsonValueNumber > sy = MakeShared<FJsonValueNumber>(s.Y);
	TSharedPtr < FJsonValueNumber > sz = MakeShared<FJsonValueNumber>(s.Z);

	TArray < TSharedPtr < FJsonValue > > sArr;
	sArr.Add(sx);
	sArr.Add(sy);
	sArr.Add(sz);

	obj->SetArrayField("s", sArr);
}

bool NadirUtil::DecodeTranslate(FVector &vt, const TSharedPtr<FJsonObject> &obj)
{
	const TArray<TSharedPtr<FJsonValue>>* valArr;
	if (!obj->TryGetArrayField("t", valArr)) return false;

	vt = ToVec3(*valArr);
	return true;
}

bool NadirUtil::DecodeRotate(FQuat &qr, const TSharedPtr<FJsonObject> &obj)
{
	const TArray<TSharedPtr<FJsonValue>>* valArr;
	if (!obj->TryGetArrayField("r", valArr)) return false;

	qr = ToQuat(*valArr);
	return true;
}

bool NadirUtil::DecodeScale(FVector &vs, const TSharedPtr<FJsonObject> &obj)
{
	const TArray<TSharedPtr<FJsonValue>>* valArr;
	if (!obj->TryGetArrayField("s", valArr)) return false;

	vs = ToVec3(*valArr);
	return true;
}

FVector NadirUtil::ToVec3(const TArray<TSharedPtr<FJsonValue>> &valArr)
{
	return FVector(valArr[0]->AsNumber(), valArr[1]->AsNumber(), valArr[2]->AsNumber() );
}

FQuat NadirUtil::ToQuat(const TArray<TSharedPtr<FJsonValue>> &valArr)
{
	return FQuat(valArr[0]->AsNumber(), valArr[1]->AsNumber(), valArr[2]->AsNumber(), valArr[3]->AsNumber() );
}

void NadirUtil::EncodeMeshComponent(TSharedRef<FJsonObject> currentObj, UStaticMeshComponent *meshComp)
{
	currentObj->SetStringField("name", meshComp->GetName());
	currentObj->SetBoolField("is_mesh", true);

	UStaticMesh* mesh = meshComp->GetStaticMesh();

	FString meshPath = mesh->GetPathName(nullptr);
	currentObj->SetStringField("mesh_name", meshPath);

	UMaterialInterface* material = meshComp->GetMaterial(0);
	if (material) {
		FString materialPath = material->GetPathName(nullptr);
		currentObj->SetStringField("material_name", materialPath);
	}
}

void NadirUtil::DecodeMeshComponent(UStaticMeshComponent *meshComp, const TSharedPtr<FJsonObject> & obj)
{
	FString meshName;
	if (obj->TryGetStringField("mesh_name", meshName)) {
		
		UStaticMesh* mt2 = (UStaticMesh*)StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *meshName);
		if(mt2) meshComp->SetStaticMesh(mt2);

		//FString realMeshName = FString::Printf(TEXT("StaticMesh'%s'"), *meshName);
		//UE_LOG(LogNadirUtil, Warning, TEXT("to find mesh %s "), *realMeshName);
/// crash at this point
		//auto meshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(*realMeshName);
		//if (meshAsset.Object != nullptr)
		//	meshComp->SetStaticMesh(meshAsset.Object);
	}
	
	FString materialName;
	if (obj->TryGetStringField("material_name", materialName)) {

		UMaterialInterface *mt3 = (UMaterialInterface*)StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *materialName);
		if (mt3) meshComp->SetMaterial(0, mt3);

	}
	
}

void NadirUtil::KeyTransformTrackSection(UMovieScene3DTransformSection *section)
{
	UE_LOG(LogNadirUtil, Log, TEXT("todo add animation to transform track section ") );
	FMovieSceneChannelProxy &channelPrx = section->GetChannelProxy();
	TArrayView < const FMovieSceneChannelMetaData > channelData = channelPrx.GetMetaData<FMovieSceneFloatChannel>();
	TArrayView<FMovieSceneFloatChannel*> channels = channelPrx.GetChannels<FMovieSceneFloatChannel>();

	FFrameNumber inFrame;
	inFrame.Value = 1 * 800;
	FFrameNumber outFrame;
	outFrame.Value = 100 * 800;
	float inValue = 0.f;
	float outValue = 100.f; 

	ERichCurveTangentMode inTangentMode = ERichCurveTangentMode::RCTM_Auto;
	ERichCurveTangentMode outTangentMode = ERichCurveTangentMode::RCTM_Auto;

	FMovieSceneTangentData inTangent;
	inTangent.ArriveTangent = 0.f;
	inTangent.LeaveTangent = 0.f;
	inTangent.TangentWeightMode = RCTWM_WeightedNone;

	FMovieSceneTangentData outTangent;
	outTangent.ArriveTangent = 0.f;
	outTangent.LeaveTangent = 0.f;
	outTangent.TangentWeightMode = RCTWM_WeightedNone;
	
	for(const FMovieSceneChannelMetaData adata : channelData) {
		FName channelName = adata.Name;
		uint8 channelOrder = adata.SortOrder;

		if(channelName.ToString() != FString("Location.X")
			&& channelName.ToString() != FString("Location.Y")
			&& channelName.ToString() != FString("Location.Z")) continue;

		UE_LOG(LogNadirUtil, Log, TEXT(" channel %s order %i "), *channelName.ToString(), channelOrder );

		FMovieSceneFloatChannel *achannel = channels[channelOrder];

		if(achannel->GetNumKeys() > 0) {
/// remove all existing keys
			achannel->Reset();
		}

		achannel->AddCubicKey(inFrame, inValue, inTangentMode, inTangent);
		achannel->AddCubicKey(outFrame, outValue, outTangentMode, outTangent);

	}

	TRange < FFrameNumber > sectionRange(inFrame, outFrame);
	section->SetRange(sectionRange);
}
