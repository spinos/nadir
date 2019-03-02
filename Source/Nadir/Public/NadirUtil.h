#pragma once

#include "LevelSequence.h"
#include "Tracks/MovieScene3DTransformTrack.h"

class NadirUtil
{
public:

	static int32 CalcFrameNumber(int32 fframe, int32 fpsUp, int32 tickUp);

	static ULevelSequence* GetActiveLevelSequence();

	static AActor *GetFirstSelectedActorName(FString &actorName);

	static UMovieScene3DTransformTrack *GetActorTransformTrack(ULevelSequence* levelSeq, const FString &actorName);

	static void CheckPossessed(ULevelSequence* levelSeq);

	static void CountLevelSequences();

	static bool CheckTrackSections(const TArray < UMovieSceneSection * > & trackSecs);

	static void LsChannels(FMovieSceneChannelProxy & channelPrx);

	static void LsKeys(const FMovieSceneFloatChannel &achannel);

	static FString InterpModeAsStr(ERichCurveInterpMode x);

	static FString TangentModeAsStr(ERichCurveTangentMode x);

	static void PrintTangent(const FMovieSceneTangentData & tangent);

};

DECLARE_LOG_CATEGORY_EXTERN(LogNadirUtil, Log, All);
