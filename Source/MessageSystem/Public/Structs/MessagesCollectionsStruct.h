#pragma once

#include "Structs/MessageStruct.h"
class UMessengerComponent;
#include "Structs/GuidArrayStruct.h"
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
	TArray<TSoftObjectPtr<UMessengerComponent>> AllMessengerComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TMap<FGuid, FMessageStruct> AllMessages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TMap<TSoftObjectPtr<UMessengerComponent>, FGuidArrayStruct> AllMessagesBySender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TMap<TSoftObjectPtr<AActor>, FGuidArrayStruct> AllMessagesByReceivingActor;

};