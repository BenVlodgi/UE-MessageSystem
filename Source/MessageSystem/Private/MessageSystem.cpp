// Copyright Epic Games, Inc. All Rights Reserved.

#include "MessageSystem.h"
#include "MessageSystemStyle.h"
#include "MessageSystemCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName MessageSystemTabName("MessageSystem");

#define LOCTEXT_NAMESPACE "FMessageSystemModule"

void FMessageSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMessageSystemStyle::Initialize();
	FMessageSystemStyle::ReloadTextures();

	FMessageSystemCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMessageSystemCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMessageSystemModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMessageSystemModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MessageSystemTabName, FOnSpawnTab::CreateRaw(this, &FMessageSystemModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FMessageSystemTabTitle", "MessageSystem"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMessageSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMessageSystemStyle::Shutdown();

	FMessageSystemCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MessageSystemTabName);
}

TSharedRef<SDockTab> FMessageSystemModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FMessageSystemModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("MessageSystem.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FMessageSystemModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MessageSystemTabName);
}

void FMessageSystemModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMessageSystemCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMessageSystemCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMessageSystemModule, MessageSystem)