// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MessageParameterValueStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct MESSAGESYSTEM_API FMessageParameterValueStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FString ParameterValue;

	// This persistently holds value to the object so the engine can track the reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Message System")
	TSoftObjectPtr<AActor> ParameterValue_AsSoftActorReference;
};
