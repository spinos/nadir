#include "NadirEdModeStyle.h"
#include "IPluginManager.h"
#include "SlateStyleRegistry.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FNadirEdModeStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )

FString FNadirEdModeStyle::InContent(const FString & RelativePath, const ANSICHAR * Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("Nadir"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FNadirEdModeStyle::StyleSet = NULL;
TSharedPtr< class ISlateStyle > FNadirEdModeStyle::Get() { return StyleSet; }

void FNadirEdModeStyle::Initialize()
{
	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	static const FVector2D Icon40x40(40.0f, 40.0f);
	
	StyleSet = MakeShared<FSlateStyleSet>(GetStyleSetName());
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	StyleSet->Set("NadirEditor.Tab", new IMAGE_BRUSH("mode_40", Icon40x40));

	const FCheckBoxStyle openTabStyle = FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::CheckBox)
		.SetUncheckedImage(IMAGE_BRUSH("open_40", Icon40x40))
		.SetUncheckedHoveredImage( IMAGE_BRUSH( "open_40", Icon40x40))
		.SetUncheckedPressedImage( IMAGE_BRUSH( "open_on_40", Icon40x40))
		.SetCheckedImage(IMAGE_BRUSH("open_on_40", Icon40x40))
		.SetCheckedHoveredImage(IMAGE_BRUSH("open_on_40", Icon40x40))
		.SetCheckedPressedImage(IMAGE_BRUSH("open_40", Icon40x40));

	StyleSet->Set("NadirEditor.OpenTab", openTabStyle);

	const FCheckBoxStyle writeTabStyle = FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::CheckBox)
		.SetUncheckedImage(IMAGE_BRUSH("write_40", Icon40x40))
		.SetUncheckedHoveredImage( IMAGE_BRUSH( "write_40", Icon40x40))
		.SetUncheckedPressedImage( IMAGE_BRUSH( "write_on_40", Icon40x40))
		.SetCheckedImage(IMAGE_BRUSH("write_on_40", Icon40x40))
		.SetCheckedHoveredImage(IMAGE_BRUSH("write_on_40", Icon40x40))
		.SetCheckedPressedImage(IMAGE_BRUSH("write_40", Icon40x40));
	
	StyleSet->Set("NadirEditor.WriteTab", writeTabStyle);

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

const FName FNadirEdModeStyle::GetStyleSetName()
{
	static const FName styleName("NadirEditorStyle");
	return styleName;
}

#undef IMAGE_BRUSH

void FNadirEdModeStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}
