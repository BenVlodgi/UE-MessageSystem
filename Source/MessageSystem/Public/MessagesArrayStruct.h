#pragma once

#include "MessageStruct.h"

//#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MessagesArrayStruct.generated.h"

USTRUCT(BlueprintType, Category = "Message System|Organization")
struct MESSAGESYSTEM_API FMessagesArrayStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TArray<FMessageStruct> Array;
};