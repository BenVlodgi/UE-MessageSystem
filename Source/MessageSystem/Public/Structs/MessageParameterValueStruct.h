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

	// The Property that holds the data. Useful for knowing what type of data we are pointing to.
	FProperty* ValueProperty;

	// Pointer to the actual data.
	void* ValuePointer;

	// Our value was last passed in directly as a string.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bLastSetFromString = true;

	// Our value was given to us by wildcard value and had its string value cached into `ValueString`.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bPointerHasBeenCachedToString = false;

	// This holds a string representation of the value, which is useful for marshaling data into anonymous function calls.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FString ValueString;

	// This persistently holds a path to the object value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Message System")
	TSoftObjectPtr<AActor> Value_AsSoftActorReference;

	// This persistently holds reference to the object so the engine doesn't GC it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Message System")
	UObject* Value_AsObject;

	// TODO: Build array of all referenced UObjects referenced by ValuePointer if it is a Struct. 
	// This to prevent GC on anything referenced by the struct alone. Like an Array of UObjects.

};

// Property Types
UENUM(BlueprintType)
enum class EPropertyTypeEnum : uint8 {
	None,
	Bool,
	Byte,
	Class,
	Enum,
	MulticastInlineDelegate,
	MulticastSparseDelegate,
	Numeric,
	Object,
	ObjectPropertyBase,
	SoftObject,
	Struct
};


UCLASS(BlueprintType)
class MESSAGESYSTEM_API UMessageParameterValueStructHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, CustomThunk, meta = (DisplayName = "Make Message Parameter Value", CustomStructureParam = "Value"), Category = "Message System")
	static void MakeMessageParameterValue(const int32& Value, FMessageParameterValueStruct& ParameterValue);
	DECLARE_FUNCTION(execMakeMessageParameterValue);

	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "Get Value", CustomStructureParam = "Value"), Category = "Message System")
	static void GetMessageParameterValue(const FMessageParameterValueStruct& Target, int32& Value, bool& IsValid);
	DECLARE_FUNCTION(execGetMessageParameterValue);

	/** Converts the value stored in a MessageParameterValueStruct to a string */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (MessageParameterValue)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Message System|Utilities")
	static FString Conv_MessageParameterValueToString(UPARAM(ref) FMessageParameterValueStruct& MessageParameterValueStruct);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct MessageParameterValue", BlueprintAutocast), Category = "Message System|Utilities")
	static FMessageParameterValueStruct Conv_StringToMessageParameterValue(EPropertyTypeEnum Type, FString String);

};