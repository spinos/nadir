/// NadirEdModeToolkit.cpp
///
/// https://answers.unrealengine.com/questions/813601/view.html
/// Sequencer.cpp
/// for (auto AttachSection : AttachTrack->GetAllSections())
///					FMovieSceneObjectBindingID ConstraintBindingID = (Cast<UMovieScene3DAttachSection>(AttachSection))->GetConstraintBindingID();
///					for (auto ParentObject : FindBoundObjects(ConstraintBindingID.GetGuid(), ConstraintBindingID.GetSequenceID()) )
///					{
///						AttachParentActor = Cast<AActor>(ParentObject.Get());
///					}

#include "NadirEdModeToolkit.h"
#include "NadirEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "EditorModeManager.h"
#include "Camera/CameraActor.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "ILevelSequenceModule.h"
#include "AssetRegistryModule.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "DesktopPlatformModule.h"
#include "EditorDirectories.h"

DEFINE_LOG_CATEGORY(LogNadir);

#define LOCTEXT_NAMESPACE "FNadirEdModeToolkit"

struct Locals
{
	static bool IsWidgetEnabled()
	{
		return GEditor->GetSelectedActors()->Num() != 0;
	}

	static TSharedRef<SEditableTextBox> MakeField()
	{
		return SNew(SEditableTextBox)
			.Text(LOCTEXT("ActorLabel", "Nothing selected"));

	}
	
	static TSharedRef<SMultiLineEditableTextBox> MakeStatsField()
	{ 
	    return SNew(SMultiLineEditableTextBox)
			.Text(LOCTEXT("MiscStats", "unknown stats"));
	}

	static TSharedRef<SWidget> MakeSendButton(FText InLabel)
	{
		return SNew(SButton)
			.Text(InLabel)
			.OnClicked_Static(&Locals::OnButtonSendClick);
	}

	static TSharedRef<SButton> MakeSaveButton(FText InLabel)
	{
		return SNew(SButton)
			.Text(InLabel)
			.OnClicked_Static(&Locals::OnButtonSaveClick);
	}

	static ULevelSequence* GetActiveLevelSequence()
	{
		TArray < UObject * > currentAssets = FAssetEditorManager::Get().GetAllEditedAssets();
		if(currentAssets.Num() < 1) {
			UE_LOG(LogNadir, Warning, TEXT("no active asset"));
			return nullptr;
		}

		ULevelSequence* levelSeq = Cast<ULevelSequence>(currentAssets[0]);
		if(!levelSeq)
		{
			UE_LOG(LogNadir, Warning, TEXT("no active level sequence"));
			return nullptr;
		}
		
		return levelSeq;
	}

	static void CountLevelSequences()
	{
		FAssetRegistryModule* assetRegModule = FModuleManager::LoadModulePtr<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
		if (!assetRegModule)
		{
			UE_LOG(LogNadir, Warning, TEXT("AssetRegistry module not loaded"));
			return;
		}
		//UE_LOG(LogNadir, Warning, TEXT("AssetRegistry memory usage = %.2f MB"), assetRegModule->Get().GetAllocatedSize() / (1024.0f * 1024.0f));
		
		IAssetRegistry& assetRegistry = assetRegModule->Get();
		TArray<FAssetData> levelSequenceAssetList;
		assetRegistry.GetAssetsByClass(ULevelSequence::StaticClass()->GetFName(), levelSequenceAssetList);

		UE_LOG(LogNadir, Log, TEXT("# level sequence asset %i "), levelSequenceAssetList.Num() );

			//for(FAssetData levelSeqAsset : levelSequenceAssetList) 
			//{
			//	ULevelSequence *levelSeq = Cast<ULevelSequence>(levelSeqAsset.GetAsset());
				/// level sequence editor
				//IAssetEditorInstance* assetEditor = FAssetEditorManager::Get().FindEditorForAsset(levelSeq, true);
			//}
		
	}

/// https://docs.unrealengine.com/en-us/Engine/Sequencer/Workflow/SequencerTimeRefactorNotes
	static int32 CalcFrameNumber(int32 fframe, int32 fpsUp, int32 tickUp)
	{
		return fframe * fpsUp / tickUp;
	}

	static void CheckPossessed(ULevelSequence* levelSeq)
	{
		UMovieScene *movScn = levelSeq->GetMovieScene();
		
		UE_LOG(LogNadir, Warning, TEXT("# possessables %i "), movScn->GetPossessableCount());
		
		for (int i = 0; i < movScn->GetPossessableCount(); i++) {
			FMovieScenePossessable & possi = movScn->GetPossessable(i);
			UE_LOG(LogNadir, Warning, TEXT("possessable %s "), *possi.GetName() );

			const FGuid & possId = possi.GetGuid();
			UE_LOG(LogNadir, Warning, TEXT("parent id is %i %i %i %i"), possId.A, possId.B, possId.C, possId.D );

		}

	}

	static AActor *GetFirstSelectedActorName(FString &actorName)
	{
/// 	int32 numSelActors = GEditor->GetSelectedActorCount();
/// 	UE_LOG(LogNadir, Warning, TEXT("# Selected Actor %i"), numSelActors );

		USelection* selActors = GEditor->GetSelectedActors();
		for (FSelectionIterator Iter(*selActors); Iter; ++Iter)
		{
			if (AActor* levelActor = Cast<AActor>(*Iter))
			{
				actorName = levelActor->GetName();
/// type of actor
				if (levelActor->IsA(ACameraActor::StaticClass())) {
					UE_LOG(LogNadir, Log, TEXT("%s is a camera "), *actorName);
				}
				return levelActor;
			}
		}

		UE_LOG(LogNadir, Error, TEXT("nothing selected") );
		return nullptr;
	} 

	static UMovieScene3DTransformTrack *GetActorTransformTrack(ULevelSequence* levelSeq, const FString &actorName)
	{
		UMovieScene *movScn = levelSeq->GetMovieScene();

		const TArray < FMovieSceneBinding > &movBinds = movScn->GetBindings();
///		UE_LOG(LogNadir, Warning, TEXT("# bindings %i "), movBinds.Num() );

		for (int i = 0; i < movBinds.Num(); i++) {

///			const TArray < UMovieSceneTrack * > & tracks = movBinds[i].GetTracks();

			const FGuid& objId = movBinds[i].GetObjectGuid();
            static const FName s_transformTrackName(TEXT("Transform"));

			UMovieScene3DTransformTrack *transformTrack = movScn->FindTrack<UMovieScene3DTransformTrack>(objId, s_transformTrackName);
			if(!transformTrack) {
				continue;
			}
			
			if(transformTrack->IsEmpty()) {
			    UE_LOG(LogNadir, Error, TEXT("empty transform track") );
			    continue;
		    }

			TArray < UObject *, TInlineAllocator < 1 > > boundObjs;
/// connection between binding id and actor reference
			levelSeq->LocateBoundObjects(objId, nullptr, boundObjs);

			if(boundObjs.Num() < 1) {
				continue;
			}

			TWeakObjectPtr<UObject> firstBoundObjPtr = boundObjs[0];
			UObject *firstBoundObj = firstBoundObjPtr.Get();
			AActor *firstActor = Cast<AActor>(firstBoundObj);

			if(!firstActor) {
				continue;
			}

			if (firstActor->GetName() == actorName) {
				return transformTrack;
			}

		}

		UE_LOG(LogNadir, Error, TEXT("cannot find transform track for %s"), *actorName );
		return nullptr;
	}

	static void ClearStatsField()
	{
		TAttribute<FText> statsAttr;
        statsAttr.Set(FText::FromString(TEXT("unknown stats")));
        MiscStatsField->SetText(statsAttr);

        SaveActBtn->SetVisibility(EVisibility::Hidden);
	}

	static FReply OnButtonSendClick()
	{
		CountLevelSequences();
		ULevelSequence* levelSeq = GetActiveLevelSequence();
		if(!levelSeq) {
			ClearStatsField();
			return FReply::Handled();
		}

		FString selActorName("unknown");

		AActor *selActor = GetFirstSelectedActorName(selActorName);

		if(!selActor) {
			ClearStatsField();
			return FReply::Handled();
		}
					
/// only works with possessables
		UMovieScene3DTransformTrack *transTrack = GetActorTransformTrack(levelSeq, selActorName);
		if(!transTrack) {
			ClearStatsField();
			return FReply::Handled();
		}
		
/// movie stats
		UMovieScene *movScn = levelSeq->GetMovieScene();

		TRange < FFrameNumber > playbackRng = movScn->GetPlaybackRange();
		FFrameNumber frameMin = playbackRng.GetLowerBoundValue();
		FFrameNumber frameMax = playbackRng.GetUpperBoundValue();
		FFrameRate tickRez = movScn->GetTickResolution();
		FFrameRate fps = movScn->GetDisplayRate();
///		UE_LOG(LogNadir, Warning, TEXT("tick resolution %i / %i "), tickRez.Numerator, tickRez.Denominator );
///		UE_LOG(LogNadir, Warning, TEXT("fps %i / %i "), fps.Numerator, fps.Denominator );
///		UE_LOG(LogNadir, Warning, TEXT("playback range %i %i "), 
///			CalcFrameNumber(frameMin.Value, fps.Numerator, tickRez.Numerator),
///			CalcFrameNumber(frameMax.Value, fps.Numerator, tickRez.Numerator) );

///		ActorNameField->SetText(FText::FromString(selActorName));
///		FText ft = ActorNameField->GetText();
///		UE_LOG(LogNadir, Warning, TEXT("Selected Actor's Name is %s"), *ft.ToString() );
///     UE_LOG(LogNadir, Warning, TEXT("active level sequence name %s"), *levelSeq->GetFName().ToString() );

        const int32 frameBegin = CalcFrameNumber(frameMin.Value, fps.Numerator, tickRez.Numerator);
		const int32 frameEnd = CalcFrameNumber(frameMax.Value, fps.Numerator, tickRez.Numerator);
		
		const TArray < UMovieSceneSection * > & trackSecs = transTrack->GetAllSections();
		 
        const FText statsStr = FText::Format(LOCTEXT("StatsText", "Actor: {0}\nLevel Sequence: {1}\nMovie Scene: playback [{2}:{3}] fps {4}\nTransform Track: #section {5}"), 
           FText::FromString(selActorName),
           FText::FromString(levelSeq->GetFName().ToString()),
           frameBegin, frameEnd, fps.Numerator,
           trackSecs.Num() ) ;
        
        TAttribute<FText> statsAttr;
        statsAttr.Set(statsStr);
        MiscStatsField->SetText(statsAttr);

        SaveActBtn->SetVisibility(EVisibility::Visible);

		return FReply::Handled();
	}

	static FReply OnButtonSaveClick()
	{
		IDesktopPlatform * desktopPlatform = FDesktopPlatformModule::Get();
		if(!desktopPlatform) {
			UE_LOG(LogNadir, Error, TEXT("cannot find desktop platform") );
			return FReply::Handled();
		}

		TArray<FString> saveFilenames;
		bool saveFileNamePicked = desktopPlatform->SaveFileDialog(
			FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
			LOCTEXT( "ExportLevelSequence", "Export Track" ).ToString(),
			*( FEditorDirectories::Get().GetLastDirectory( ELastDirectory::GENERIC_EXPORT ) ),
			TEXT( "" ),
			TEXT( "Text document|*.txt" ),
			EFileDialogFlags::None,
			saveFilenames );

		if(!saveFileNamePicked) {
			UE_LOG(LogNadir, Error, TEXT("file dialog aborted") );
			return FReply::Handled();
		}

		const FString &saveFilename = saveFilenames[0];
		FEditorDirectories::Get().SetLastDirectory( ELastDirectory::GENERIC_EXPORT, FPaths::GetPath( saveFilename ) ); // Save path as default for next time.

		UE_LOG(LogNadir, Log, TEXT("save file to %s"), *saveFilename );
		return FReply::Handled();
	}

///	static TSharedPtr<SEditableTextBox> ActorNameField;
	static TSharedPtr<SMultiLineEditableTextBox> MiscStatsField;
	static TSharedPtr<SButton> SaveActBtn;
	
};

///TSharedPtr<SEditableTextBox> Locals::ActorNameField;
TSharedPtr<SMultiLineEditableTextBox> Locals::MiscStatsField;
TSharedPtr<SButton> Locals::SaveActBtn;

FNadirEdModeToolkit::FNadirEdModeToolkit()
{
}

void FNadirEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	///TSharedRef<SEditableTextBox> actorNameField = Locals::MakeField();
	TSharedRef<SMultiLineEditableTextBox> miscStatsField = Locals::MakeStatsField();
	TSharedRef<SButton> saveActBtn = Locals::MakeSaveButton(LOCTEXT("SaveButtonLabel", "Export Track"));
	saveActBtn->SetVisibility(EVisibility::Hidden);
	
/// ref to ptr conversion
	Locals::MiscStatsField = miscStatsField;
	Locals::SaveActBtn = saveActBtn;

	//SAssignNew(ToolkitWidget, SBorder)
		//.HAlign(HAlign_Center)
	SAssignNew(ToolkitWidget, SScrollBox )
		+SScrollBox::Slot()
		.Padding(25)
		//.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(15)
			[
				SNew(STextBlock)
				.AutoWrapText(true)
				.Text(LOCTEXT("HelperLabel", "Select some actor and show statistics below"))
			]
			/*
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("UpButtonLabel", "Up"), FVector(0, 0, Factor))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("DownButtonLabel", "Down"), FVector(0, 0, -Factor))
				]
				
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5)
				[
					SNew(STextBlock)
						.Text(LOCTEXT("ActorFieldLabel", "Actor"))
				]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						actorNameField
					]	
				]
				*/
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeSendButton(LOCTEXT("SendButtonLabel", "Load Selected"))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoHeight()
				[
					miscStatsField
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					saveActBtn
				]
		];
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FNadirEdModeToolkit::GetToolkitFName() const
{
	return FName("NadirEdMode");
}

FText FNadirEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("NadirEdModeToolkit", "DisplayName", "NadirEdMode Tool");
}

class FEdMode* FNadirEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FNadirEdMode::EM_NadirEdModeId);
}

#undef LOCTEXT_NAMESPACE
