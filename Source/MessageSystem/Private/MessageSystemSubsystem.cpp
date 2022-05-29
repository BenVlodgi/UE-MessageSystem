// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageSystemSubsystem.h"
#include "MessengerComponent.h"

void UMessageSystemSubsystem::MessengerComponentUpdated(UMessengerComponent* MessengerComponent)
{
	if (IsValid(MessengerComponent))
	{
		UWorld* world = MessengerComponent->GetWorld();
		if (IsValid(world))
		{
			EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
			FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);

			//Only do this once, the first time the component is modified. This will also happen in a newly duplicated actor/component
			int index = messagesCollections.AllMessengerComponents.Find(MessengerComponent);
			if (index < 0)
			{
				if (MessengerComponent->MessageEvents.Num() > 0)
				{
					for (int i = 0; i < MessengerComponent->MessageEvents.Num(); i++)
					{

						// The actor may have been duplicated, and its messages will be pointing to the previous senders, we need to update those.
						AActor* SendingActor = MessengerComponent->GetOwner();
						if (MessengerComponent->MessageEvents[i].SendingActor != SendingActor 
						 || MessengerComponent->MessageEvents[i].SendingComponent != MessengerComponent)
						{
							MessengerComponent->MessageEvents[i].SendingActor = MessengerComponent->GetOwner();
							MessengerComponent->MessageEvents[i].SendingComponent = MessengerComponent;

							// This component/actor was likely copied which is why the sender didn't match.
							// We will get a new ID for our message, so it won't have the same ID as its copy-source.
							MessengerComponent->MessageEvents[i].ID = FGuid::NewGuid();
						}

						// If this has a bad Guid, lets generate a new one.
						// This could happen if someone added the event manually to the component.
						if (!MessengerComponent->MessageEvents[i].ID.IsValid())
						{
							MessengerComponent->MessageEvents[i].ID = FGuid::NewGuid(); // bbbb <-- Annette typed those
						}

						// Add to cache.
						AddMessage(MessengerComponent->MessageEvents[i], false); // Don't broadcast, we'll handle that.
					}
				}
				else // If we have no messages to add, make sure we at-least track the component. (Normally this would happen in the AddMessage)
				{
					messagesCollections.AllMessengerComponents.AddUnique(MessengerComponent);

					// Ensure to commit changes made here
					MessagesCollectionsByWorld.Add(worldType, messagesCollections);
				}

				OnMessengerComponentUpdated.Broadcast(MessengerComponent);
			}
		}
	}

}

void UMessageSystemSubsystem::MessengerComponentRemoved(UMessengerComponent* MessengerComponent)
{
	if (IsValid(MessengerComponent))
	{
		UWorld* world = MessengerComponent->GetWorld();
		if (IsValid(world))
		{
			EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
			FMessagesCollectionsStruct* messagesCollections = MessagesCollectionsByWorld.Find(worldType);
			if (messagesCollections)
			{
				// Remove these messages. Reverse because this affects the array we are iterating over
				for (int i = MessengerComponent->MessageEvents.Num()- 1; i >= 0 ; i--)
				{
					RemoveMessage(MessengerComponent->MessageEvents[i], false); // Don't broadcast, we'll handle that.
				}

			}
		}
	}
	OnMessengerComponentRemoved.Broadcast(MessengerComponent);
}

void UMessageSystemSubsystem::AddMessage(FMessageStruct Message, bool BroadcastUpdate)
{
	if (Message.SendingComponent.IsValid())
	{
		UMessengerComponent* messengerComponent = Message.SendingComponent.Get();
		if (IsValid(messengerComponent)) // This check may not be nessesary
		{
			UWorld* world = messengerComponent->GetWorld();
			if (IsValid(world))
			{
				EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
				FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);

				messagesCollections.AllMessengerComponents.AddUnique(messengerComponent);
				messagesCollections.AllMessages.Add(Message.ID, Message);

				FGuidArrayStruct messagesBySender = messagesCollections.AllMessagesBySender.FindOrAdd(messengerComponent);
				messagesBySender.Array.AddUnique(Message.ID);

				FGuidArrayStruct messagesByReceivingActor = messagesCollections.AllMessagesByReceivingActor.FindOrAdd(Message.TargetActor);
				messagesByReceivingActor.Array.AddUnique(Message.ID);
				
				// Ensure to commit changes made here
				messagesCollections.AllMessagesBySender.Add(messengerComponent, messagesBySender);
				messagesCollections.AllMessagesByReceivingActor.Add(Message.TargetActor, messagesByReceivingActor);
				MessagesCollectionsByWorld.Add(worldType, messagesCollections);

				if (BroadcastUpdate)
				{
					OnMessengerComponentUpdated.Broadcast(messengerComponent);
				}
			}
		}
	}
}

void UMessageSystemSubsystem::RemoveMessage(FMessageStruct Message, bool BroadcastUpdate)
{
	if (Message.SendingComponent.IsValid())
	{
		UMessengerComponent* messengerComponent = Message.SendingComponent.Get(); // Only using this to get the correct world
		if (IsValid(messengerComponent)) // This check may not be nessesary
		{
			UWorld* world = messengerComponent->GetWorld();
			if (IsValid(world))
			{
				EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
				FMessagesCollectionsStruct* messagesCollections = MessagesCollectionsByWorld.Find(worldType);

				if (messagesCollections)
				{

					FMessageStruct* cachedMessage = messagesCollections->AllMessages.Find(Message.ID);

					TSoftObjectPtr<UMessengerComponent> cachedSendingComponent;
					if (cachedMessage)
					{
						cachedSendingComponent = cachedMessage->SendingComponent;
					}

					messagesCollections->AllMessages.Remove(Message.ID);

					FGuidArrayStruct* messagesBySender = messagesCollections->AllMessagesBySender.Find(cachedSendingComponent);
					if (messagesBySender)
					{
						messagesBySender->Array.Remove(Message.ID);
					}

					FGuidArrayStruct* messagesByReceivingActor = messagesCollections->AllMessagesByReceivingActor.Find(cachedMessage->TargetActor);
					if (messagesByReceivingActor)
					{
						messagesByReceivingActor->Array.Remove(Message.ID);
					}
				}

				if (BroadcastUpdate)
				{
					OnMessengerComponentUpdated.Broadcast(messengerComponent);
				}
			}
		}
	}
}

void UMessageSystemSubsystem::UpdateMessage(FMessageStruct Message, bool BroadcastUpdate)
{
	if (Message.SendingComponent.IsValid())
	{
		UMessengerComponent* messengerComponent = Message.SendingComponent.Get();
		if (IsValid(messengerComponent)) // This check may not be nessesary
		{
			RemoveMessage(Message, false);
			AddMessage(Message, false);

			if (BroadcastUpdate)
			{
				OnMessengerComponentUpdated.Broadcast(messengerComponent);
			}
		}
	}
}
