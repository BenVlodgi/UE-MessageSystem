// Copyright 2022 Dream Seed LLC.
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message System|Organization")
	TMap<FGuid, FMessageStruct> AllMessages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message System|Organization")
	TMap<TSoftObjectPtr<UMessengerComponent>, FGuidArrayStruct> AllMessagesBySender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message System|Organization")
	TMap<TSoftObjectPtr<AActor>, FGuidArrayStruct> AllMessagesByReceivingActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message System|Organization")
	TArray<TSoftObjectPtr<UMessengerComponent>> AllMessengerComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message System|Organization", AdvancedDisplay)
	TArray<TSoftObjectPtr<UMessengerComponent>> PendingAddMessengerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message System|Organization", AdvancedDisplay)
	TArray<TSoftObjectPtr<UMessengerComponent>> PendingRemoveMessengerComponent;

};