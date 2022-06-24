// Copyright 2022 Dream Seed LLC.
#pragma once

#include "Misc/Guid.h"
#include "UObject/NoExportTypes.h"
#include "GuidArrayStruct.generated.h"

USTRUCT(BlueprintType, Category = "Message System|Organization")
struct MESSAGESYSTEM_API FGuidArrayStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TArray<FGuid> Array;
};