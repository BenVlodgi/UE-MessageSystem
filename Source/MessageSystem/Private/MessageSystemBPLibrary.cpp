// Copyright 2022 Dream Seed LLC.


#include "MessageSystemBPLibrary.h"
#include "MessageSystem.h"
#include "DynamicWildcardLibrary.h"

DEFINE_LOG_CATEGORY(LogMessageSystem);

UMessageSystemBPLibrary::UMessageSystemBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

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

EWorldTypeEnum UMessageSystemBPLibrary::GetWorldType(UObject* Object)
{
	if (IsValid(Object))
	{
		switch (Object->GetWorld()->WorldType)
		{
			case EWorldType::None: return EWorldTypeEnum::None;
			case EWorldType::Game: return EWorldTypeEnum::Game;
			case EWorldType::Editor: return EWorldTypeEnum::Editor;
			case EWorldType::PIE: return EWorldTypeEnum::PIE;
			case EWorldType::EditorPreview: return EWorldTypeEnum::EditorPreview;
			case EWorldType::GamePreview: return EWorldTypeEnum::GamePreview;
			case EWorldType::GameRPC: return EWorldTypeEnum::GameRPC;
			case EWorldType::Inactive: return EWorldTypeEnum::Inactive;
		}
	}

	return EWorldTypeEnum::None;
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