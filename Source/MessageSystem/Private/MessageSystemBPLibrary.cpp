// Copyright Dream Seed LLC


#include "MessageSystemBPLibrary.h"
#include "MessageSystem.h"

DEFINE_LOG_CATEGORY(LogMessageSystem);

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
#if WITH_EDITORONLY_DATA //HasMetaData is editor only
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
#endif
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

bool UMessageSystemBPLibrary::CallFunctionByNameWithArguments(UObject* Target, FName FunctionName, FMessageParametersStruct Parameters, FMessageParametersStruct& ReturnValues)
{
	const TCHAR* Str = TEXT("");
	FOutputDevice& Ar = *GLog;
	UObject* Executor = NULL;
	bool bForceCallWithNonExec = true;
	//bool UObject::CallFunctionByNameWithArguments(const TCHAR * Str, FOutputDevice & Ar, UObject * Executor, bool bForceCallWithNonExec/*=false*/)
	{
		//// Find an exec function.
		//FString MsgStr;
		//if (!FParse::Token(Str, MsgStr, true))
		//{
		//	UE_LOG(LogMessageSystem, Verbose, TEXT("CallFunctionByNameWithArguments: Not Parsed '%s'"), Str);
		//	return false;
		//}
		//const FName Message = FName(*MsgStr, FNAME_Find);
		const FName Message = FunctionName;
		//if (Message == NAME_None)
		//{
		//	UE_LOG(LogMessageSystem, Verbose, TEXT("CallFunctionByNameWithArguments: Name not found '%s'"), Str);
		//	return false;
		//}
		UFunction* Function = Target->FindFunction(Message);
		if (nullptr == Function)
		{
			UE_LOG(LogMessageSystem, Verbose, TEXT("CallFunctionByNameWithArguments: Function not found '%s'"), Str);
			return false;
		}
		if (0 == (Function->FunctionFlags & FUNC_Exec) && !bForceCallWithNonExec)
		{
			UE_LOG(LogMessageSystem, Verbose, TEXT("CallFunctionByNameWithArguments: Function not executable '%s'"), Str);
			return false;
		}

		FProperty* LastParameter = nullptr;

		// find the last parameter
		for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It)
		{
			LastParameter = *It;
		}

		// Parse all function parameters.
		uint8* Params = (uint8*)FMemory_Alloca(Function->ParmsSize); // UE4
		//uint8* Params = (uint8*)FMemory_Alloca_Aligned(Function->ParmsSize, Function->GetMinAlignment()); // UE5
		FMemory::Memzero(Params, Function->ParmsSize);

		for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			FProperty* LocalProp = *It;
			checkSlow(LocalProp);
			if (!LocalProp->HasAnyPropertyFlags(CPF_ZeroConstructor))
			{
				LocalProp->InitializeValue_InContainer(Params);
			}
		}

		const uint32 ExportFlags = PPF_None;
		bool bFailed = false;
		int32 NumParamsEvaluated = 0;
		for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It, NumParamsEvaluated++)
		{
			FProperty* PropertyParam = *It;
			checkSlow(PropertyParam); // Fix static analysis warning
			if (NumParamsEvaluated == 0 && Executor)
			{
				FObjectPropertyBase* Op = CastField<FObjectPropertyBase>(*It);
				if (Op && Executor->IsA(Op->PropertyClass))
				{
					// First parameter is implicit reference to object executing the command.
					Op->SetObjectPropertyValue(Op->ContainerPtrToValuePtr<uint8>(Params), Executor);
					continue;
				}
			}

			// Keep old string around in case we need to pass the whole remaining string
			const TCHAR* RemainingStr = Str;

			// Parse a new argument out of Str
			FString ArgStr;
			FParse::Token(Str, ArgStr, true);

			// if ArgStr is empty but we have more params to read parse the function to see if these have defaults, if so set them
			bool bFoundDefault = false;
			bool bFailedImport = true;


#if WITH_EDITOR
			if (false) // Don't do this now that we're executing. If ever do this, do it somehow on compile to collect the defaults. Also want Blueprint Defaults
			{
				if (!FCString::Strcmp(*ArgStr, TEXT("")))
				{
					const FName DefaultPropertyKey(*(FString(TEXT("CPP_Default_")) + PropertyParam->GetName()));
					const FString& PropertyDefaultValue = Function->GetMetaData(DefaultPropertyKey);
					if (!PropertyDefaultValue.IsEmpty())
					{
						bFoundDefault = true;

						const TCHAR* Result = It->ImportText(*PropertyDefaultValue, It->ContainerPtrToValuePtr<uint8>(Params), ExportFlags, NULL);
						bFailedImport = (Result == nullptr);
					}
				}
			}
#endif

			{
				FName Key = FName(PropertyParam->GetName());
				FMessageParameterValueStruct* MessageParameterValue = Parameters.Parameters.Find(Key);
				if (MessageParameterValue)
				{
					UE_LOG(LogTemp, Log, TEXT("MessageSystem: Found Property: %s"), *Key.ToString());
					const FString& PropertyValue = MessageParameterValue->ParameterValue_AsSoftActorReference.IsNull()
						? MessageParameterValue->ParameterValue
						: MessageParameterValue->ParameterValue_AsSoftActorReference.ToString();

					const TCHAR* Result = It->ImportText(*PropertyValue, It->ContainerPtrToValuePtr<uint8>(Params), ExportFlags, NULL);
					bFailedImport = (Result == nullptr);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("MessageSystem: Not found Property: %s"), *Key.ToString());
				}
			}


			//if (!bFoundDefault)
			//{
			//	// if this is the last string property and we have remaining arguments to process, we have to assume that this
			//	// is a sub-command that will be passed to another exec (like "cheat giveall weapons", for example). Therefore
			//	// we need to use the whole remaining string as an argument, regardless of quotes, spaces etc.
			//	if (PropertyParam == LastParameter && PropertyParam->IsA<FStrProperty>() && FCString::Strcmp(Str, TEXT("")) != 0)
			//	{
			//		ArgStr = FString(RemainingStr).TrimStart();
			//	}
			//
			//	const TCHAR* Result = It->ImportText(*ArgStr, It->ContainerPtrToValuePtr<uint8>(Params), ExportFlags, NULL);
			//	bFailedImport = (Result == nullptr);
			//}

			if (bFailedImport)
			{
				FFormatNamedArguments Arguments;
				Arguments.Add(TEXT("Message"), FText::FromName(Message));
				Arguments.Add(TEXT("PropertyName"), FText::FromName(It->GetFName()));
				Arguments.Add(TEXT("FunctionName"), FText::FromName(Function->GetFName()));
				Ar.Logf(TEXT("%s"), *FText::Format(NSLOCTEXT("Core", "BadProperty", "'{Message}': Bad or missing property '{PropertyName}' when trying to call {FunctionName}"), Arguments).ToString());
				bFailed = true;

				continue;
			}
		}

		//if (!bFailed)
		{
			Target->ProcessEvent(Function, Params);
		}

		//!!destructframe see also UObject::ProcessEvent
		for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			It->DestroyValue_InContainer(Params);
		}

		// Success.
		return true;
	}
	return false;
}

FString UMessageSystemBPLibrary::MessageToString(FMessageStruct message)
{
	return FString(TEXT(""));

	//return FString(TEXT("Message: %s : %s : %s"), 
	//	IsValid(message.SendingActor) ? message.SendingActor->GetName() : "Invalid",
	//	message.OnTrigger,
	//	message.TargetActor.IsValid() ? message.TargetActor.GetAssetName() : "Invalid"
	//);
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