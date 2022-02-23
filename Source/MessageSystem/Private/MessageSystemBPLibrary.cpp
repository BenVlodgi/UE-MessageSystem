// Copyright Epic Games, Inc. All Rights Reserved.

#include "MessageSystemBPLibrary.h"
#include "MessageSystem.h"

UMessageSystemBPLibrary::UMessageSystemBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


void UMessageSystemBPLibrary::GetClassInterfaces(UClass* Class, TArray<UClass*>& Interfaces)
{
	Interfaces.Empty();
	
	if (Class)
	{
		//return Class->Interfaces
		for (FImplementedInterface Interface : Class->Interfaces)
		{
			Interfaces.Add(Interface.Class);
		}
	}
}

//void UMessageSystemBPLibrary::GetAllObjectFunctions(const UObject* Object) {
//
//	for (TFieldIterator<UFunction> FIT(Object->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
//
//		UFunction* Function = *FIT;
//		UE_LOG(LogTemp, Log, TEXT("Function Found:  %s();"), *Function->GetName());
//
//	}
//}

void UMessageSystemBPLibrary::GetInterfaceFunctionNames(const UClass* Class, TArray<FString>& FunctionNames)
{
	FunctionNames.Empty();
	for (TFieldIterator<UFunction> FIT(Class); FIT; ++FIT)
	{
		UFunction* Function = *FIT;
		FunctionNames.Add(Function->GetName());
	}
}

void UMessageSystemBPLibrary::GetInterfaceFunctionNamesWithKeyword(const UClass* Class, FString keyword, TArray<FString>& FunctionNames)
{
	FunctionNames.Empty();
	for (TFieldIterator<UFunction> FIT(Class); FIT; ++FIT)
	{
		UFunction* Function = *FIT;
		if (Function->HasMetaData(TEXT("KeyWords")))
		{
			FString FunctionKeyWords = *Function->FindMetaData(TEXT("KeyWords"))->ToLower();
			if (FunctionKeyWords.Contains(keyword.ToLower()))
			{
				FunctionNames.Add(Function->GetName());
			}
		}
	}
}

void UMessageSystemBPLibrary::GetClassFunctionProperies(const UClass* Class, FString FunctionName, TArray<FString>& PropertyNames)
{
	PropertyNames.Empty();
	for (TFieldIterator<UFunction> FIT(Class); FIT; ++FIT)
	{
		UFunction* Function = *FIT;
		if (Function->GetName() == FunctionName)
		{
			for (TFieldIterator<FProperty>PIT(Function); PIT; ++PIT)
			{
				PropertyNames.Add(PIT->GetName());
			}
		}
	}
}

/*
void UMessageSystemBPLibrary::GetClassFunctionPropertyDefaultValue (const UClass* Class, FString FunctionName, TArray<FString>& PropertyNames)
{
	PropertyNames.Empty();
	//for (TFieldIterator<UFunction> FIT(Class); FIT; ++FIT)
	//{
	//	UFunction* Function = *FIT;
	//	if (Function->GetName() == FunctionName)
	//	{
	//		for (TFieldIterator<FProperty>PIT(Function); PIT; ++PIT)
	//		{
	//			PropertyNames.Add(PIT->GetName());
	//			PIT->
	//				//TCppType GetDefaultPropertyValue()
	//		}
	//	}
	//}
}
//*/