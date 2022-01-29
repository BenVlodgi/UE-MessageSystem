// Copyright Epic Games, Inc. All Rights Reserved.

#include "MessageSystemCommands.h"

#define LOCTEXT_NAMESPACE "FMessageSystemModule"

void FMessageSystemCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "MessageSystem", "Bring up MessageSystem window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
