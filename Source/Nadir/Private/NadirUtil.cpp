///
/// https://answers.unrealengine.com/questions/813601/view.html

#include "NadirUtil.h"
#include "AssetRegistryModule.h"
#include "Channels/MovieSceneChannelProxy.h"

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
