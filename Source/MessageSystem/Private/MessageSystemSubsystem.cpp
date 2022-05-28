// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageSystemSubsystem.h"

#include "MessageStruct.h"
#include "MessengerComponent.h"

void UMessageSystemSubsystem::MessengerComponentUpdated(UMessengerComponent* MessengerComponent)
{
	// TODO: make sure we update the correct one.

	if (IsValid(MessengerComponent))
	{
		UWorld* world = MessengerComponent->GetWorld();
		if (IsValid(world))
		{
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

				// Trying to commit changes made here
				messagesCollections.AllMessagesByReceivingActor.Add(message.TargetActor, targetMessagesArray);
			}

			// Trying to commit changes made here
			MessagesCollectionsByWorld.Add(worldType, messagesCollections);
		}
	}

	OnMessengerComponentUpdated.Broadcast(MessengerComponent);
}

void UMessageSystemSubsystem::MessengerComponentRemoved(UMessengerComponent* MessengerComponent)
{
	// TODO: Make sure we remove this component. It might be null already.

	if (IsValid(MessengerComponent))
	{
		UWorld* world = MessengerComponent->GetWorld();
		if (IsValid(world))
		{
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
	}

	OnMessengerComponentRemoved.Broadcast(MessengerComponent);
}

void UMessageSystemSubsystem::AddMessage(FMessageStruct Message)
{

	//if (IsValid(Message.SendingComponent))
	//{
	//	UWorld* world = MessengerComponent->GetWorld();
	//	if (IsValid(world))
	//	{
	//		EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
	//		FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);
	//
	//		// Remove existing messages for this component from the AllMessagesByReceivingActor dictionary.
	//		FMessagesArrayStruct messagesArray = messagesCollections.AllMessagesBySender.FindOrAdd(MessengerComponent);
	//		for (FMessageStruct message : messagesArray.Array)
	//		{
	//			FMessagesArrayStruct targetMessagesArray = messagesCollections.AllMessagesByReceivingActor.FindOrAdd(message.TargetActor);
	//
	//			// Reverse loop over, because we may be removing from this list
	//			for (int i = targetMessagesArray.Array.Max(); i >= 0; i--)
	//			{
	//				FMessageStruct targetMessage = targetMessagesArray.Array[i];
	//				if (targetMessage.SendingComponent == MessengerComponent)
	//				{
	//					targetMessagesArray.Array.RemoveAt(i);
	//				}
	//			}
	//		}
	//
	//		// Remove messages from AllMessagesBySender dictionary.
	//		messagesCollections.AllMessagesBySender.Remove(MessengerComponent);
	//
	//
	//
	//		// Add messages for this component to the AllMessagesBySender & AllMessagesBySender dictionaries.
	//		messagesCollections.AllMessengerComponents.AddUnique(MessengerComponent);
	//
	//		for (FMessageStruct message : MessengerComponent->MessageEvents)
	//		{
	//			messagesArray.Array.Add(message);
	//
	//			FMessagesArrayStruct targetMessagesArray = messagesCollections.AllMessagesByReceivingActor.FindOrAdd(message.TargetActor);
	//			targetMessagesArray.Array.Add(message);
	//
	//			// Trying to commit changes made here
	//			messagesCollections.AllMessagesByReceivingActor.Add(message.TargetActor, targetMessagesArray);
	//		}
	//
	//		// Trying to commit changes made here
	//		MessagesCollectionsByWorld.Add(worldType, messagesCollections);
	//	}
	//}
	//
	//OnMessengerComponentUpdated.Broadcast(MessengerComponent);
}