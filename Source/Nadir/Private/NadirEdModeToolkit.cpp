// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NadirEdModeToolkit.h"
#include "NadirEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"

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

	static FReply OnButtonSendClick()
	{
		FString actorName("unknown");
		USelection* SelectedActors = GEditor->GetSelectedActors();
		for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
		{
			if (AActor* LevelActor = Cast<AActor>(*Iter))
			{
				actorName = LevelActor->GetName();
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
						.Text(LOCTEXT("ActorFieldLabel", "Actor Name"))
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
