/*
 *  NadirEdModeWidget.cpp
 *
 *  2019/5/30
 */

#include "NadirEdModeWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "NadirUtil.h"
#include "NadirEdModeStyle.h"
#include "Engine/Selection.h"
#include "EditorModeManager.h"
#include "Camera/CameraActor.h"
#include "ILevelSequenceModule.h"
#include "DesktopPlatformModule.h"
#include "MeshDescription.h"
#include "MeshAttributes.h"
#include "MeshAttributeArray.h"
#include "EditorDirectories.h"
#include "RawMesh.h"
#include "AssetRegistryModule.h"
#include "NadirActor.h"
#include "NadirSceneComponent.h"
#include "exampleLib/ExampleLibrary.h"
#include "exampleLib/ExampleLibraryData.h"

#define LOCTEXT_NAMESPACE "FNadirEdModeWidget"

void NadirEdModeWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.HAlign(HAlign_Left)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
				[
					makeRadioPanel()
				]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
				[
					makeMainPanel()
				]
		]
	];
}

TSharedRef<SGridPanel> NadirEdModeWidget::makeRadioPanel()
{
	m_selectedTab = TabRadioWrite;

	return SNew(SGridPanel)
		+ SGridPanel::Slot(0, 0)
		//.FillColumn(0, 0.5f)
		//.HAlign(HAlign_Left)
		.Padding(0.0f, 2.0f)
			[
				SNew(SCheckBox)
				.Style(FNadirEdModeStyle::Get(), TEXT("NadirEditor.WriteTab"))
				.IsChecked(this, &NadirEdModeWidget::handleTabRadioIsChecked, TabRadioWrite)
				.OnCheckStateChanged(this, &NadirEdModeWidget::handleTabRadioCheckStateChanged, TabRadioWrite)	
			]

		+ SGridPanel::Slot(1, 0)
			.Padding(0.0f, 2.0f)
			[
				SNew(SCheckBox)
				.Style(FNadirEdModeStyle::Get(), TEXT("NadirEditor.OpenTab"))
				.IsChecked(this, &NadirEdModeWidget::handleTabRadioIsChecked, TabRadioOpen)
				.OnCheckStateChanged(this, &NadirEdModeWidget::handleTabRadioCheckStateChanged, TabRadioOpen)
			];
}

TSharedRef<SWidgetSwitcher> NadirEdModeWidget::makeMainPanel()
{
	SAssignNew(m_panelSelector, SWidgetSwitcher);
	m_panelSelector->AddSlot()
		[
			makeWritePanel()
		];
	m_panelSelector->AddSlot()
		[
			makeReadPanel()
		];

	m_panelSelector->SetActiveWidgetIndex(0);
	return m_panelSelector.ToSharedRef();
}

ECheckBoxState NadirEdModeWidget::handleTabRadioIsChecked( TabRadioChoice ButtonId ) const
{
	return (m_selectedTab == ButtonId) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void NadirEdModeWidget::handleTabRadioCheckStateChanged( ECheckBoxState NewRadioState, TabRadioChoice RadioThatChanged )
{
	if (NewRadioState == ECheckBoxState::Checked)
	{
		m_selectedTab = RadioThatChanged;
	}
	m_panelSelector->SetActiveWidgetIndex(m_selectedTab);
}

TSharedRef<SScrollBox> NadirEdModeWidget::makeWritePanel()
{
	m_miscStatsField = SNew(SMultiLineEditableTextBox)
			.Text(LOCTEXT("MiscStats", "unknown stats"));

	m_saveActBtn = SNew(SButton)
			.Text(LOCTEXT("SaveButtonLabel", "Export Track"))
			.OnClicked(this, &NadirEdModeWidget::OnButtonSaveClick);

	m_saveActBtn->SetVisibility(EVisibility::Hidden);

	return SNew(SScrollBox )
			+SScrollBox::Slot()
 		.HAlign(HAlign_Left)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
				[
					SNew(STextBlock)
					.AutoWrapText(false)
					.WrapTextAt(270.0f)
					.Text(LOCTEXT("WriteHelperLabel", "Select some actor and show statistics below"))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SButton)
					.Text(LOCTEXT("SendButtonLabel", "Load Selected"))
					.OnClicked(this, &NadirEdModeWidget::OnButtonSendClick)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoHeight()
				[
					m_miscStatsField.ToSharedRef()
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					m_saveActBtn.ToSharedRef()
				]
		];
}

TSharedRef<SScrollBox> NadirEdModeWidget::makeReadPanel()
{
	return SNew(SScrollBox )
			+SScrollBox::Slot()
 		.HAlign(HAlign_Left)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
				[
					SNew(STextBlock)
					.AutoWrapText(false)
					.WrapTextAt(270.0f)
					.Text(LOCTEXT("ReadHelperLabel", "Some test funcs of panel 2"))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoHeight()
				[
					SNew(SButton)
					.Text(LOCTEXT("TestAnimationButtonLabel", "Test Animation"))
					.OnClicked(this, &NadirEdModeWidget::OnButtonTestAnimationClick)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SButton)
					.Text(LOCTEXT("TestRotationButtonLabel", "Test Rotation"))
					.OnClicked(this, &NadirEdModeWidget::OnButtonTestRotationClick)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SButton)
					.Text(LOCTEXT("CreateMeshLabel", "Create Mesh"))
					.OnClicked(this, &NadirEdModeWidget::OnButtonCreateMeshClick)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SButton)
					.Text(LOCTEXT("CreateActorLabel", "Create Actor"))
					.OnClicked(this, &NadirEdModeWidget::OnButtonCreateActorClick)
				]
		];
}

FReply NadirEdModeWidget::OnButtonSaveClick()
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
	d.setNameAge("foxbat", 49);
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

FReply NadirEdModeWidget::OnButtonSendClick()
{
	NadirUtil::CountLevelSequences();
	ULevelSequence* levelSeq = NadirUtil::GetActiveLevelSequence();
	if(!levelSeq) {
		resetStatsField();
		return FReply::Handled();
	}

	FString selActorName("unknown");

	AActor *selActor = NadirUtil::GetFirstSelectedActorName(selActorName);

	if(!selActor) {
		resetStatsField();
		return FReply::Handled();
	}

	FString actorTypeName("unknown");
	NadirUtil::GetActorTypeName(actorTypeName, selActor);
				
/// only works with possessables
	UMovieScene3DTransformTrack *transTrack = NadirUtil::GetActorTransformTrack(levelSeq, selActorName);
	if(!transTrack) {
		resetStatsField();
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
	 
    const FText statsStr = FText::Format(LOCTEXT("StatsText", "Actor: {0} type {1}\nLevel Sequence: {2}\nMovie Scene: playback [{3}:{4}] fps {5}\nTransform Track: #section {6}"), 
       FText::FromString(selActorName),
		FText::FromString(actorTypeName),
       FText::FromString(levelSeq->GetFName().ToString()),
       frameBegin, frameEnd, fps.Numerator,
       trackSecs.Num() ) ;
    
    TAttribute<FText> statsAttr;
    statsAttr.Set(statsStr);
    m_miscStatsField->SetText(statsAttr);

    NadirUtil::CheckTrackSections(trackSecs);

    m_saveActBtn->SetVisibility(EVisibility::Visible);

	return FReply::Handled();
}

FReply NadirEdModeWidget::OnButtonTestRotationClick()
{
	USelection* selActors = GEditor->GetSelectedActors();
	if (selActors->Num() < 1) {
		UE_LOG(LogNadirUtil, Error, TEXT("select an actor ") );
		return FReply::Handled();
	}

	FSelectionIterator Iter(*selActors);
	AActor* levelActor = Cast<AActor>(*Iter);
	FString actorName = levelActor->GetName();
	UE_LOG(LogNadirUtil, Log, TEXT("actor name %s "), *actorName);

	FTransform tm = levelActor->GetActorTransform();
	FQuat q = tm.GetRotation();
	UE_LOG(LogNadirUtil, Log, TEXT("rotation quaternion %s "), *q.ToString());

	FVector vrot = q.Euler();
	UE_LOG(LogNadirUtil, Log, TEXT("rotation euler %s "), *vrot.ToString());

/// x and y is flipped
	double rx = FMath::DegreesToRadians<double>(vrot.X) * -1.0;
	double ry = FMath::DegreesToRadians<double>(vrot.Y) * -1.0;
	double rz = FMath::DegreesToRadians<double>(vrot.Z);

	FQuat qx(FVector::ForwardVector, (float)rx);
	FQuat qy(FVector::RightVector, (float)ry);
	FQuat qz(FVector::UpVector, (float)rz);

	UE_LOG(LogNadirUtil, Log, TEXT("qx %s "), *qx.ToString());
	UE_LOG(LogNadirUtil, Log, TEXT("qy %s "), *qy.ToString());
	UE_LOG(LogNadirUtil, Log, TEXT("qz %s "), *qz.ToString());

/// right to left
	FQuat qRecon = qz * qy * qx;
	UE_LOG(LogNadirUtil, Warning, TEXT("quaternion from euler %s recon error %f"), *qRecon.ToString(),
		FQuat::Error(qRecon, q));

	FVector vrecon = qRecon.Euler();
	UE_LOG(LogNadirUtil, Log, TEXT("recon euler %s"), *vrecon.ToString());

/// left hand 
///   z                                   z
///   |                                   |
///    ---x  rotate 90 deg around z   y---   front is x
///  /                                   /
/// y                                   x
/// x: roll y:pitch z:yaw
/// yaw first rool last
/// right hand
///    y
///    |
///     ---x
///   /
///  z
/// x unchanged
/// swap y and z, y rotate backwards
/// mirror the quaternion
/// (roll, -yaw, pitch) 

	return FReply::Handled();
}

FReply NadirEdModeWidget::OnButtonCreateMeshClick()
{
	NadirUtil::CreateMinimalMesh();
	
	return FReply::Handled();
}

FReply NadirEdModeWidget::OnButtonCreateActorClick()
{
	UWorld *world = NadirUtil::GetEditorWorld();
	if (!world) {
		UE_LOG(LogNadirUtil, Warning, TEXT("not an editor world "));
		return FReply::Handled();
	}

	auto hier = world->SpawnActor<ANadirActor>();
/// or? Cast<ANadirActor>(GEditor->AddActor(World->GetCurrentLevel(), ANadirActor::StaticClass(), initialTransform));

	UNadirSceneComponent* root = Cast<UNadirSceneComponent>(hier->GetRootComponent());

	UNadirSceneComponent* child1 = NewObject<UNadirSceneComponent>(hier, "Child1");

	child1->AttachToComponent(root, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	
	child1->CreationMethod = EComponentCreationMethod::Instance;
	child1->RegisterComponent();

	UStaticMesh * mesh = NadirUtil::CreateMinimalMesh();
	UStaticMeshComponent *meshComp = NewObject<UStaticMeshComponent>(hier, "meshShape");
	meshComp->SetStaticMesh(mesh);

	meshComp->AttachToComponent(child1, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	meshComp->CreationMethod = EComponentCreationMethod::Instance;
	meshComp->RegisterComponent();

	hier->PostEditChange();

	return FReply::Handled();
}

void NadirEdModeWidget::resetStatsField()
{
	TAttribute<FText> statsAttr;
    statsAttr.Set(FText::FromString(TEXT("unknown stats")));
    m_miscStatsField->SetText(statsAttr);

    m_saveActBtn->SetVisibility(EVisibility::Hidden);
}

FReply NadirEdModeWidget::OnButtonTestAnimationClick()
{
	NadirUtil::CountLevelSequences();
	ULevelSequence* levelSeq = NadirUtil::GetActiveLevelSequence();
	if(!levelSeq) {
		return FReply::Handled();
	}

	FString selActorName("unknown");
	AActor *selActor = NadirUtil::GetFirstSelectedActorName(selActorName);

	if(!selActor) {
		return FReply::Handled();
	}

	NadirUtil::CreateActorTransformTrack(levelSeq, selActorName);
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
