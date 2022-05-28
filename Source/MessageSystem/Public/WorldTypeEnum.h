#pragma once


//#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WorldTypeEnum.generated.h"

// Specifies the goal/source of a UWorld object
UENUM(BlueprintType)
enum class EWorldTypeEnum : uint8 {
	/** An untyped world, in most cases this will be the vestigial worlds of streamed in sub-levels */
	None,

	/** The game world */
	Game,

	/** A world being edited in the editor */
	Editor,

	/** A Play In Editor world */
	PIE,

	/** A preview world for an editor tool */
	EditorPreview,

	/** A preview world for a game */
	GamePreview,

	/** A minimal RPC world for a game */
	GameRPC,

	/** An editor world that was loaded but not currently being edited in the level editor */
	Inactive
};