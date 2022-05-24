// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageSystemSubsystem.h"

#include "MessageStruct.h"
#include "MessengerComponent.h"

void UMessageSystemSubsystem::MessengerComponentUpdated(UMessengerComponent* MessengerComponent)
{
	if (!MessengerComponent)
		return;

	UWorld* world = MessengerComponent->GetWorld();
	if (!world)
		return;

	EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
	FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);

	// Remove existing messages for this component from the AllMessagesByReceivingActor dictionary.
	FMessagesArrayStruct messagesArray = messagesCollections.AllMessagesBySender.FindOrAdd(MessengerComponent);
	for (FMessageStruct message : messagesArray.Array)
	{
		FMessagesArrayStruct targetMessagesArray = messagesCollections.AllMessagesByReceivingActor.FindOrAdd(message.TargetActor);

		// Reverse loop over, because we may be removing from this list
		for (int i = targetMessagesArray.Array.Max(); i >= 0; i--)
		{
			FMessageStruct targetMessage = targetMessagesArray.Array[i];
			if (targetMessage.SendingComponent == MessengerComponent)
			{
				targetMessagesArray.Array.RemoveAt(i);
			}
		}
	}

	// Remove messages from AllMessagesBySender dictionary.
	messagesCollections.AllMessagesBySender.Remove(MessengerComponent);



	// Add messages for this component to the AllMessagesBySender & AllMessagesBySender dictionaries.
	messagesCollections.AllMessengerComponents.AddUnique(MessengerComponent);

	for (FMessageStruct message : MessengerComponent->MessageEvents)
	{
		messagesArray.Array.Add(message);

		FMessagesArrayStruct targetMessagesArray = messagesCollections.AllMessagesByReceivingActor.FindOrAdd(message.TargetActor);
		targetMessagesArray.Array.Add(message);
	}

	// Trying to commit changes made here
	MessagesCollectionsByWorld.Add(worldType, messagesCollections);
}

void UMessageSystemSubsystem::MessengerComponentRemoved(UMessengerComponent* MessengerComponent)
{
	// Remove all outputs from this component

	if (!MessengerComponent)
		return;

	UWorld* world = MessengerComponent->GetWorld();
	if (!world)
		return;

	EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
	FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);

	// Remove existing messages for this component from the AllMessagesByReceivingActor dictionary.
	FMessagesArrayStruct messagesArray = messagesCollections.AllMessagesBySender.FindOrAdd(MessengerComponent);
	for (FMessageStruct message : messagesArray.Array)
	{
		FMessagesArrayStruct targetMessagesArray = messagesCollections.AllMessagesByReceivingActor.FindOrAdd(message.TargetActor);

		// Reverse loop over, because we may be removing from this list
		for (int i = targetMessagesArray.Array.Max(); i >= 0; i--)
		{
			FMessageStruct targetMessage = targetMessagesArray.Array[i];
			if (targetMessage.SendingComponent == MessengerComponent)
			{
				targetMessagesArray.Array.RemoveAt(i);
			}
		}
	}

	// Remove messages from AllMessagesBySender dictionary.
	messagesCollections.AllMessagesBySender.Remove(MessengerComponent);

	// Remove component from list of components
	messagesCollections.AllMessengerComponents.Remove(MessengerComponent);
}