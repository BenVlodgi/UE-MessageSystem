#pragma once

#include "MessageStruct.h"
class UMessengerComponent;
#include "MessagesArrayStruct.h"
#include "Misc/Guid.h"
//class AActor;

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MessagesCollectionsStruct.generated.h"

USTRUCT(BlueprintType, Category = "Message System|Organization")
struct MESSAGESYSTEM_API FMessagesCollectionsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TArray<UMessengerComponent*> AllMessengerComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TMap<FGuid, FMessagesArrayStruct> AllMessages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TMap<UMessengerComponent*, FMessagesArrayStruct> AllMessagesBySender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TMap<TSoftObjectPtr<AActor>, FMessagesArrayStruct> AllMessagesByReceivingActor;
};