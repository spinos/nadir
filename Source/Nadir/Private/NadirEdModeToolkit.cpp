// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NadirEdModeToolkit.h"
#include "NadirEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"
#include "Camera/CameraActor.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "ILevelSequenceModule.h"
#include "AssetRegistryModule.h"

DEFINE_LOG_CATEGORY(LogNadirToolkit);

#define LOCTEXT_NAMESPACE "FNadirEdModeToolkit"

FNadirEdModeToolkit::FNadirEdModeToolkit()
{
}

struct Locals
{
	static bool IsWidgetEnabled()
	{
		return GEditor->GetSelectedActors()->Num() != 0;
	}

	static FReply OnButtonClick(FVector InOffset)
	{
		USelection* SelectedActors = GEditor->GetSelectedActors();

		// Let editor know that we're about to do something that we want to undo/redo
		GEditor->BeginTransaction(LOCTEXT("MoveActorsTransactionName", "MoveActors"));

		// For each selected actor
		for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
		{
			if (AActor* LevelActor = Cast<AActor>(*Iter))
			{
				// Register actor in opened transaction (undo/redo)
				LevelActor->Modify();
				// Move actor to given location
				LevelActor->TeleportTo(LevelActor->GetActorLocation() + InOffset, FRotator(0, 0, 0));

				FString actorName = LevelActor->GetName();
				UE_LOG(LogNadirToolkit, Warning, TEXT("Actor's Name is %s"), *actorName);
			}
		}

		// We're done moving actors so close transaction
		GEditor->EndTransaction();

		UE_LOG(LogNadirToolkit, Verbose, TEXT("Button is clicked"));

		return FReply::Handled();
	}

	static TSharedRef<SWidget> MakeButton(FText InLabel, const FVector InOffset)
	{
		return SNew(SButton)
			.Text(InLabel)
			.OnClicked_Static(&Locals::OnButtonClick, InOffset);
	}

	static TSharedRef<SEditableTextBox> MakeField()
	{
		return SNew(SEditableTextBox)
			.Text(LOCTEXT("ActorLabel", "Nothing selected"));

	}

	static TSharedRef<SWidget> MakeSendButton(FText InLabel)
	{
		return SNew(SButton)
			.Text(InLabel)
			.OnClicked_Static(&Locals::OnButtonSendClick);
	}

	static ULevelSequence* GetActiveLevelSequence()
	{
		TArray < UObject * > currentAssets = FAssetEditorManager::Get().GetAllEditedAssets();
		if(currentAssets.Num() < 1) {
			UE_LOG(LogNadirToolkit, Warning, TEXT("no active asset"));
			return nullptr;
		}

		ULevelSequence* levelSeq = Cast<ULevelSequence>(currentAssets[0]);
		if(!levelSeq)
		{
			UE_LOG(LogNadirToolkit, Warning, TEXT("no active level sequence"));
			return nullptr;
		}

		return levelSeq;
	}

	static void CountLevelSequences()
	{
		FAssetRegistryModule* assetRegModule = FModuleManager::LoadModulePtr<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
		if (!assetRegModule)
		{
			UE_LOG(LogNadirToolkit, Warning, TEXT("AssetRegistry module not loaded"));
			return;
		}
		//UE_LOG(LogNadirToolkit, Warning, TEXT("AssetRegistry memory usage = %.2f MB"), assetRegModule->Get().GetAllocatedSize() / (1024.0f * 1024.0f));
		
		IAssetRegistry& assetRegistry = assetRegModule->Get();
		TArray<FAssetData> levelSequenceAssetList;
		assetRegistry.GetAssetsByClass(ULevelSequence::StaticClass()->GetFName(), levelSequenceAssetList);

		UE_LOG(LogNadirToolkit, Warning, TEXT("# level sequence asset %i "), levelSequenceAssetList.Num() );

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

	static void CheckPossessed(UMovieScene *movScn)
	{
		UE_LOG(LogNadirToolkit, Warning, TEXT("# possessables %i "), movScn->GetPossessableCount());
		for (int i = 0; i < movScn->GetPossessableCount(); i++) {
			FMovieScenePossessable & possi = movScn->GetPossessable(i);
			UE_LOG(LogNadirToolkit, Warning, TEXT("possessable %s "), *possi.GetName() );
		}

		const TArray < FMovieSceneBinding > &movBinds = movScn->GetBindings();
		UE_LOG(LogNadirToolkit, Warning, TEXT("# bindings %i "), movBinds.Num() );
		for (int i = 0; i < movBinds.Num(); i++) {
			UE_LOG(LogNadirToolkit, Warning, TEXT("bind %s "), *movBinds[i].GetName() );

			const FGuid& guid = movBinds[i].GetObjectGuid();
			const TArray < UMovieSceneTrack * > & tracks = movBinds[i].GetTracks();

            UE_LOG(LogNadirToolkit, Warning, TEXT("# tracks %i "), tracks.Num() );

		}
		
	}

	static FReply OnButtonSendClick()
	{
		CountLevelSequences();
		ULevelSequence* levelSeq = GetActiveLevelSequence();
		if(!levelSeq) {
			return FReply::Handled();
		}
		UMovieScene *movScn = levelSeq->GetMovieScene();
		CheckPossessed(movScn);

		TRange < FFrameNumber > playbackRng = movScn->GetPlaybackRange();
		FFrameNumber frameMin = playbackRng.GetLowerBoundValue();
		FFrameNumber frameMax = playbackRng.GetUpperBoundValue();
		FFrameRate tickRez = movScn->GetTickResolution();
		UE_LOG(LogNadirToolkit, Warning, TEXT("tick resolution %i / %i "), tickRez.Numerator, tickRez.Denominator );
		FFrameRate fps = movScn->GetDisplayRate();
		UE_LOG(LogNadirToolkit, Warning, TEXT("fps %i / %i "), fps.Numerator, fps.Denominator );
		UE_LOG(LogNadirToolkit, Warning, TEXT("playback range %i %i "), 
			CalcFrameNumber(frameMin.Value, fps.Numerator, tickRez.Numerator),
			CalcFrameNumber(frameMax.Value, fps.Numerator, tickRez.Numerator) );

/// a few UEditorEngine functions

		USelection* selComps = GEditor->GetSelectedComponents();
		int32 numSelComps = 0;
		for (FSelectionIterator Iter(*selComps); Iter; ++Iter)
		{
			numSelComps++;
		}
		UE_LOG(LogNadirToolkit, Warning, TEXT("# Selected Component %i"), numSelComps );

		int32 numSelActors = GEditor->GetSelectedActorCount();
		UE_LOG(LogNadirToolkit, Warning, TEXT("# Selected Actor %i"), numSelActors );

		FString actorName("unknown");
		USelection* SelectedActors = GEditor->GetSelectedActors();
		for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
		{
			if (AActor* LevelActor = Cast<AActor>(*Iter))
			{
				actorName = LevelActor->GetName();

/// type of actor
				if (LevelActor->IsA(ACameraActor::StaticClass()))
				{
					UE_LOG(LogNadirToolkit, Warning, TEXT("%s is a camera "), *actorName );
				}

				UE_LOG(LogNadirToolkit, Warning, TEXT("actor label is %s "), *LevelActor->GetActorLabel() );
			}
		}
		ActorNameField->SetText(FText::FromString(actorName));
		FText ft = ActorNameField->GetText();
		UE_LOG(LogNadirToolkit, Warning, TEXT("Selected Actor's Name is %s"), *ft.ToString() );

		return FReply::Handled();
	}

	static TSharedPtr<SEditableTextBox> ActorNameField;

};

TSharedPtr<SEditableTextBox> Locals::ActorNameField;

void FNadirEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{

	const float Factor = 256.0f;

	TSharedRef<SEditableTextBox> actorNameField = Locals::MakeField();
/// ref to ptr conversion
	Locals::ActorNameField = actorNameField;

	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(50)
			[
				SNew(STextBlock)
				.AutoWrapText(true)
				.Text(LOCTEXT("HelperLabel", "Select some actors and move them around using buttons below"))
			]
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
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						Locals::MakeButton(LOCTEXT("LeftButtonLabel", "Left"), FVector(0, -Factor, 0))
					]
					+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							Locals::MakeButton(LOCTEXT("RightButtonLabel", "Right"), FVector(0, Factor, 0))
						]
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
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeSendButton(LOCTEXT("SendButtonLabel", "Send"))
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
