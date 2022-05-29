// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "MessengerComponent.h"
class UMessengerComponent; // Forward Declaration can be used for pointers.
#include "Structs/MessageParametersStruct.h"
#include "Misc/Guid.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MessageStruct.generated.h"


USTRUCT(BlueprintType)
struct MESSAGESYSTEM_API FMessageStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FGuid ID;

	// Actor the message originates from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TSoftObjectPtr<AActor> SendingActor;

	// Component who sent/broadcast this message
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TSoftObjectPtr<UMessengerComponent> SendingComponent;

	// Name of triggering event on sending component/actor. The reason this message is being sent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FName OnTrigger;

	// Parameters associated with the triggering event.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FMessageParametersStruct TriggerParameters;

	// Send this message to this actor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TSoftObjectPtr<AActor> TargetActor;

	// Event message name to send. This can also be a function name on the actor. If the name contains a period, then it is targeting a component to send the message to or call function with name i.e. "ComponentName.FunctionName".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FName SendEvent;

	// Parameters for this message event. If the message is calling a function, these parameters will be used to fill the function parameter values.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FMessageParametersStruct EventParameters;

	// How long after the triggering event before the message is sent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	float Delay;

	// Delayed Actions will not be sent if the Sender is destroyed before the delay time has elapsed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bCancelIfSenderIsDestroyed;

	// If this message will be sent when the triggering event is broadcast.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bEnabled = true;

	//// This message only can fire once.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	//bool bOnlyOnce = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	//bool RestartDelay = [Block Until Fired, Retrigger Delay Cooldown, Queue Multiple];
	
};