// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MessageSystemStyle.h"

class FMessageSystemCommands : public TCommands<FMessageSystemCommands>
{
public:

	FMessageSystemCommands()
		: TCommands<FMessageSystemCommands>(TEXT("MessageSystem"), NSLOCTEXT("Contexts", "MessageSystem", "MessageSystem Plugin"), NAME_None, FMessageSystemStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};