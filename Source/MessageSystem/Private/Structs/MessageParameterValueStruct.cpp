// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Structs/MessageParameterValueStruct.h"




DEFINE_FUNCTION(UMessageParameterValueStructHelper::execMakeMessageParameterValue)
{
	//P_GET_STRUCT_REF(FMessageParameterValueStruct, Z_Param_Target);
	//FMessageParameterValueStruct& MessageParameterValueStruct = Z_Param_Target;
	//
	//P_GET_STRUCT(FName, Z_Param_Name);
	//FName Name = Z_Param_Name;

	Stack.StepCompiledIn<FStructProperty>(NULL);
	void* ValuePropertyAddress = Stack.MostRecentPropertyAddress;
	FProperty* ValueProperty = Stack.MostRecentProperty;


	//MessageParameterValueStruct
	//WildcardMap.Values.Add(GameplayTag, SrcPropertyAddr);

	P_GET_STRUCT_REF(FMessageParameterValueStruct, Z_Param_ParameterValue);
	Z_Param_ParameterValue.ValueProperty = ValueProperty;
	Z_Param_ParameterValue.ValuePointer = ValuePropertyAddress;
	Z_Param_ParameterValue.ValueString = "Empty";
	Z_Param_ParameterValue.bLastSetFromString = false;
	Z_Param_ParameterValue.bPointerHasBeenCachedToString = false;

	if (ValueProperty)
	{
		Z_Param_ParameterValue.ValueString = ValueProperty->GetClass()->GetName();
		if (ValuePropertyAddress)
		{
		}
	}

	P_FINISH;
	P_NATIVE_BEGIN;
	P_NATIVE_END;
}



DEFINE_FUNCTION(UMessageParameterValueStructHelper::execGetMessageParameterValue)
{
	P_GET_STRUCT_REF(FMessageParameterValueStruct, Z_Param_Target);
	FMessageParameterValueStruct& Target = Z_Param_Target;



	
	FProperty* ValueProperty = Z_Param_Target.ValueProperty;
	void* Value = Z_Param_Target.ValuePointer;

	Stack.MostRecentPropertyAddress = NULL;
	void* Z_Param_Out_Value_Temp = nullptr;

	if (Stack.Code)
	{
		Stack.Step(Stack.Object, Z_Param_Out_Value_Temp);
	}
	else
	{
		checkSlow(CastField<TProperty>(PropertyChainForCompiledIn) && CastField<FProperty>(PropertyChainForCompiledIn));
		FProperty* Property = (FProperty*)Stack.PropertyChainForCompiledIn;
		Stack.PropertyChainForCompiledIn = Property->Next;
		Stack.StepExplicitProperty(Z_Param_Out_Value_Temp, Property);
	}

	FProperty* SrcProperty = CastField<FProperty>(Stack.MostRecentProperty);
	bool bCompatiblePropertyType = false;
	if (ValueProperty && Value && SrcProperty)
	{
		if (ValueProperty->GetClass()->IsChildOf(SrcProperty->GetClass()))
		{
			bCompatiblePropertyType = true;
			SrcProperty->CopyCompleteValueFromScriptVM((Stack.MostRecentPropertyAddress != NULL) ? (void*)(Stack.MostRecentPropertyAddress) : (void*)Z_Param_Out_Value_Temp, Value);
		}
	}

	P_GET_UBOOL_REF(Z_Param_Out_IsValid);
	Z_Param_Out_IsValid = bCompatiblePropertyType;

	P_FINISH;
	P_NATIVE_BEGIN;
	P_NATIVE_END;
}

FString UMessageParameterValueStructHelper::Conv_MessageParameterValueToString(FMessageParameterValueStruct& MessageParameterValueStruct)
{
	FString returnValue;
	if (MessageParameterValueStruct.bLastSetFromString || MessageParameterValueStruct.bPointerHasBeenCachedToString)
	{
		returnValue = MessageParameterValueStruct.ValueString;
	}
	else if (MessageParameterValueStruct.ValueProperty)
	{
		MessageParameterValueStruct.ValueProperty->ExportTextItem(returnValue, MessageParameterValueStruct.ValuePointer, nullptr, nullptr, 0);
		MessageParameterValueStruct.ValueString = returnValue;
		MessageParameterValueStruct.bPointerHasBeenCachedToString = true;
	}

	if (returnValue.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("MessageSystem: Unable to convert MessageParameterValue to string: %s"), MessageParameterValueStruct.ValueProperty);
		returnValue = TEXT("");
	}

	return returnValue;
}

FMessageParameterValueStruct UMessageParameterValueStructHelper::Conv_StringToMessageParameterValue(EPropertyTypeEnum Type, FString String)
{
	FMessageParameterValueStruct returnValue;

	returnValue.ValueString = String;

	//switch (Type)
	//{
	//	//EmitTerm(Statement.LHS, (FProperty*)(GetDefault<FBoolProperty>()));
	//	case EPropertyTypeEnum::None: returnValue.ParameterValueProperty = NULL; break;
	//	case EPropertyTypeEnum::Bool: returnValue.ParameterValueProperty = Cast<FBoolProperty>(FBoolProperty::StaticClass()->GetDefaultObject()); break;
	//	case EPropertyTypeEnum::Byte:
	//	case EPropertyTypeEnum::Class:
	//	case EPropertyTypeEnum::Enum:
	//	case EPropertyTypeEnum::MulticastInlineDelegate:
	//	case EPropertyTypeEnum::MulticastSparseDelegate: returnValue.ParameterValueProperty = NULL;
	//	case EPropertyTypeEnum::Numeric: returnValue.ParameterValueProperty = Cast<FNumericProperty>(FNumericProperty::StaticClass()->GetDefaultObject()); break;
	//	case EPropertyTypeEnum::Object:
	//	case EPropertyTypeEnum::ObjectPropertyBase:
	//	case EPropertyTypeEnum::SoftObject:
	//	case EPropertyTypeEnum::Struct:
	//	default: returnValue.ParameterValueProperty = NULL;
	//}
	//
	//if (returnValue.ParameterValueProperty)
	//{
	//	const TCHAR* Result = returnValue.ParameterValueProperty->ImportText(*String, returnValue.ParameterValueProperty->ContainerPtrToValuePtr<uint8>(&returnValue), PPF_None, NULL);
	//	if (!Result)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Conv_StringToMessageParameterValue Failed"));
	//	}
	//}
	return returnValue;
}
