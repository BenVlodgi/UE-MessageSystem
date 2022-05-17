// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "MessengerComponent.h"
class UMessengerComponent;

#include "ValueStoreStruct.h"
#include "MessageParametersStruct.h"


#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MessageStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct MESSAGESYSTEM_API FMessageStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	AActor* SendingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	UMessengerComponent* SendingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FName OnTrigger;

	//// Parameters from sender
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	//FValueStoreStruct TriggerParameters; // Should TriggerParameters be merged with EventParameters?

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TWeakObjectPtr<AActor> TargetActorSoft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FName SendEvent;

	// Parameters prepared for the event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FMessageParametersStruct EventParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	float Delay; // Should the delay be in waiting to send, or in waiting to process once received. 

	// Delayed (Latent) Actions will not be sent if the Sender is destroyed before the delay time has elapsed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bCancelIfSenderIsDestroyed;
	
	
};
