// Copyright 2022 Dream Seed LLC.

#pragma once

#include "Modules/ModuleManager.h"

class FMessageSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
