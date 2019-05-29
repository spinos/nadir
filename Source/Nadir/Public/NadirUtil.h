/*
 *  NadirUtil.h
 * 
 *  2019/5/30
 */

#pragma once

#include "LevelSequence.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"

class NadirUtil
{
public:

	static int32 CalcFrameNumber(int32 fframe, int32 fpsUp, int32 tickUp);

	static ULevelSequence* GetActiveLevelSequence();

	static AActor *GetFirstSelectedActorName(FString &actorName);

	static UMovieScene3DTransformTrack *GetActorTransformTrack(ULevelSequence* levelSeq, const FString &actorName);

	static void CreateActorTransformTrack(ULevelSequence* levelSeq, const FString &actorName);

	static void CheckPossessed(ULevelSequence* levelSeq);

	static void CountLevelSequences();

	static bool CheckTrackSections(const TArray < UMovieSceneSection * > & trackSecs);

	static void LsChannels(FMovieSceneChannelProxy & channelPrx);

	static void LsKeys(const FMovieSceneFloatChannel &achannel);

	static FString InterpModeAsStr(ERichCurveInterpMode x);

	static FString TangentModeAsStr(ERichCurveTangentMode x);

	static void PrintTangent(const FMovieSceneTangentData & tangent);

	static void GetActorTypeName(FString &typeName, const AActor *actor);

	static UStaticMesh* CreateMinimalMesh();

	static UWorld* GetEditorWorld();

	static void EncodeTranslate(TSharedRef<FJsonObject> obj, const FVector &t);

	static void EncodeRotate(TSharedRef<FJsonObject> obj, const FQuat &q);

	static void EncodeScale(TSharedRef<FJsonObject> obj, const FVector &s);

	static bool DecodeTranslate(FVector &vt, const TSharedPtr<FJsonObject> & obj);

	static bool DecodeRotate(FQuat &qr, const TSharedPtr<FJsonObject> & obj);

	static bool DecodeScale(FVector &vs, const TSharedPtr<FJsonObject> & obj);

	static FVector ToVec3(const TArray<TSharedPtr<FJsonValue>> &valArr);

	static FQuat ToQuat(const TArray<TSharedPtr<FJsonValue>> &valArr);

	static void EncodeMeshComponent(TSharedRef<FJsonObject> currentObj, UStaticMeshComponent *meshComp);

	static void DecodeMeshComponent(UStaticMeshComponent *meshComp, const TSharedPtr<FJsonObject> & obj);

	static void KeyTransformTrackSection(UMovieScene3DTransformSection *section);

};

DECLARE_LOG_CATEGORY_EXTERN(LogNadirUtil, Log, All);
