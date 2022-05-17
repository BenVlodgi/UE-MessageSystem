// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MessageParameterValueStruct.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MessageParametersStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct MESSAGESYSTEM_API FMessageParametersStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TMap<FName, FMessageParameterValueStruct> Parameters;
};