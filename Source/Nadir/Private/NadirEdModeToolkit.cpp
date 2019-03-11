/// NadirEdModeToolkit.cpp

#include "NadirEdModeToolkit.h"
#include "NadirEdMode.h"
#include "NadirUtil.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "EditorModeManager.h"
#include "Camera/CameraActor.h"
#include "ILevelSequenceModule.h"
#include "DesktopPlatformModule.h"
#include "EditorDirectories.h"
#include "exampleLib/ExampleLibrary.h"
#include "exampleLib/ExampleLibraryData.h"

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

	static void ClearStatsField()
	{
		TAttribute<FText> statsAttr;
        statsAttr.Set(FText::FromString(TEXT("unknown stats")));
        MiscStatsField->SetText(statsAttr);

        SaveActBtn->SetVisibility(EVisibility::Hidden);
	}

	static FReply OnButtonSendClick()
	{
		NadirUtil::CountLevelSequences();
		ULevelSequence* levelSeq = NadirUtil::GetActiveLevelSequence();
		if(!levelSeq) {
			ClearStatsField();
			return FReply::Handled();
		}

		FString selActorName("unknown");

		AActor *selActor = NadirUtil::GetFirstSelectedActorName(selActorName);

		if(!selActor) {
			ClearStatsField();
			return FReply::Handled();
		}
					
/// only works with possessables
		UMovieScene3DTransformTrack *transTrack = NadirUtil::GetActorTransformTrack(levelSeq, selActorName);
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

        const int32 frameBegin = NadirUtil::CalcFrameNumber(frameMin.Value, fps.Numerator, tickRez.Numerator);
		const int32 frameEnd = NadirUtil::CalcFrameNumber(frameMax.Value, fps.Numerator, tickRez.Numerator);
		
		const TArray < UMovieSceneSection * > & trackSecs = transTrack->GetAllSections();
		 
        const FText statsStr = FText::Format(LOCTEXT("StatsText", "Actor: {0}\nLevel Sequence: {1}\nMovie Scene: playback [{2}:{3}] fps {4}\nTransform Track: #section {5}"), 
           FText::FromString(selActorName),
           FText::FromString(levelSeq->GetFName().ToString()),
           frameBegin, frameEnd, fps.Numerator,
           trackSecs.Num() ) ;
        
        TAttribute<FText> statsAttr;
        statsAttr.Set(statsStr);
        MiscStatsField->SetText(statsAttr);

        NadirUtil::CheckTrackSections(trackSecs);

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

		ExampleLibraryData d;
		d._name = "foxtrap";
		d._age = 43;
		ExampleLibraryFunction(d);

		std::string tmpDir("D:/tmp");
		bool isTmpFolder = ExampleLibraryTestDirectory(tmpDir);
		if (isTmpFolder) {
			UE_LOG(LogNadir, Log, TEXT("D:/tmp is a folder"));
		}
		else {
			UE_LOG(LogNadir, Log, TEXT("D:/tmp is not a folder"));
		}

		std::string hdfFileName("D:/foo.hes");
		bool isHdfSaved = ExampleLibraryTestHdf(hdfFileName);
		if (isHdfSaved) {
			UE_LOG(LogNadir, Log, TEXT("hdf5 is saved"));
		}
		else {
			UE_LOG(LogNadir, Log, TEXT("hdf5 is not saved"));
		}

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
	return NSLOCTEXT("NadirEdModeToolkit", "DisplayName", "Nadir Tool");
}

class FEdMode* FNadirEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FNadirEdMode::EM_NadirEdModeId);
}

#undef LOCTEXT_NAMESPACE
