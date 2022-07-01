// Copyright 2022 Dream Seed LLC.

#pragma once

//#include "MessengerComponent.h"
class UMessengerComponent; // Forward Declaration can be used for pointers.
#include "DynamicParametersStruct.h"
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

	// Component who sent/broadcast this message
	// Note: This value is not set while in Sending Component's array. It is filled in when used externally, like when message is sent. This is becauses soft references will get in the way when trying to delete the owning actor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TSoftObjectPtr<UMessengerComponent> SendingComponent;

	// Name of triggering event on sending component/actor. The reason this message is being sent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FName OnTrigger;

	// Parameters associated with the triggering event.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FDynamicParametersStruct TriggerParameters;

	// These parameters are specifically allowed or blocked from passing through in event call as function parameters. 
	// Default behavior if not found is: True: Send this parameter.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TMap<FName, bool> TriggerParametersPassThrough;

	// TODO: Enum
	// Message Target Type
	// -1: Default
	//  0: Actor
	//  1: Component
	//  2: Global
	//  3: System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	int TargetType = -1;

	// Send this message to this actor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TSoftObjectPtr<AActor> TargetActor;

	// Event message name to send. This can also be a function name on the actor. If the name contains a period, then it is targeting a component to send the message to or call function with name i.e. "ComponentName.FunctionName".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FName SendEvent;

	// Parameters for this message event. If the message is calling a function, these parameters will be used to fill the function parameter values.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	FDynamicParametersStruct EventParameters;

	// These parameters are specifically marked for sending as function parameters (This will override a trigger parameter of the same name).
	// Default behavior if not found is: True: Send this parameter.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TMap<FName, bool> EventParametersEnabled;

	// How long after the triggering event before the message is sent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	float Delay;

	// Delayed Actions will not be sent if the Sender is destroyed before the delay time has elapsed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bCancelIfSenderIsDestroyed;

	// If this message will be sent when the triggering event is broadcast.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bEnabled = true;

	// Send Trigger Parameters as Event Parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	bool bForwardTriggerParameters = true;

	//// This message only can fire once.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	//bool bOnlyOnce = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	//bool RestartDelay = [Block Until Fired, Retrigger Delay Cooldown, Queue Multiple];
	
};