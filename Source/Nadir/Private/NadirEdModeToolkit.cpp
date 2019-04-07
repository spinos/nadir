/// NadirEdModeToolkit.cpp

#include "NadirEdModeToolkit.h"
#include "NadirEdMode.h"
#include "NadirUtil.h"
#include "NadirEdModeStyle.h"
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
#include "MeshDescription.h"
#include "MeshAttributes.h"
#include "MeshAttributeArray.h"
#include "EditorDirectories.h"
#include "RawMesh.h"
#include "AssetRegistryModule.h"

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

	static TSharedRef<SButton> MakeTestRotationButton(FText InLabel)
	{
		return SNew(SButton)
			.Text(InLabel)
			.OnClicked_Static(&Locals::OnButtonTestRotationClick);
	}

	static TSharedRef<SButton> MakeCreateMeshButton(FText InLabel)
	{
		return SNew(SButton)
			.Text(InLabel)
			.OnClicked_Static(&Locals::OnButtonCreateMeshClick);
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

		FString actorTypeName("unknown");
		NadirUtil::GetActorTypeName(actorTypeName, selActor);
					
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
		 
        const FText statsStr = FText::Format(LOCTEXT("StatsText", "Actor: {0} type {1}\nLevel Sequence: {2}\nMovie Scene: playback [{3}:{4}] fps {5}\nTransform Track: #section {6}"), 
           FText::FromString(selActorName),
			FText::FromString(actorTypeName),
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

	static FReply OnButtonTestRotationClick()
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

	static FReply OnButtonCreateMeshClick()
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
		
		//package->FullyLoad();

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
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeTestRotationButton(LOCTEXT("TestRotationButtonLabel", "Test Rotation"))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeCreateMeshButton(LOCTEXT("CreateMeshLabel", "Create Mesh"))
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
