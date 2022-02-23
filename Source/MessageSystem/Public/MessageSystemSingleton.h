// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MessageSystemSingleton.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MESSAGESYSTEM_API UMessageSystemSingleton : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite)
	TMap<UClass*, UObject*> MessageInterfaceImplementationsClassToObject;
	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MessageSystem")
	static UMessageSystemSingleton* GetMessageSystemInstance();
	
};
