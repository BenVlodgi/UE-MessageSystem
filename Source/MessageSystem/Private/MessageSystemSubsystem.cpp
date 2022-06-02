// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/Engine.h"

#include "MessageSystemSubsystem.h"
#include "MessengerComponent.h"

void UMessageSystemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->OnLevelActorAdded().AddLambda([this](AActor* AddedActor) { this->OnLevelActorAdded(AddedActor); });
		GEngine->OnLevelActorDeleted().AddLambda([this](AActor* DestroyedActor) { this->OnLevelActorDeleted(DestroyedActor); });
	}
#endif
}

#if WITH_EDITOR
void UMessageSystemSubsystem::OnLevelActorAdded(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Called: OnLevelActorAdded: %s"), *Actor->GetName());
	return;
	//if (IsValid(Actor))
	//{
	//	TArray<UMessengerComponent*> messengerComponents;
	//	Actor->GetComponents<UMessengerComponent>(messengerComponents);
	//
	//	for (int i = messengerComponents.Num() - 1; i >= 0; i--)
	//	{
	//		QueueMessengerComponentAdded(messengerComponents[i]);
	//	}
	//}
}

void UMessageSystemSubsystem::OnLevelActorDeleted(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Called: OnLevelActorDeleted: %s"), *Actor->GetName());
	//return;
	if (IsValid(Actor))
	{
		TArray<UMessengerComponent*> messengerComponents;
		Actor->GetComponents<UMessengerComponent>(messengerComponents);
		
		for (int i = messengerComponents.Num() - 1; i >= 0; i--)
		{
			MessengerComponentRemoved(messengerComponents[i]);
		}
	}
}
#endif

void UMessageSystemSubsystem::QueueMessengerComponentAdded(UMessengerComponent* MessengerComponent)
{
	if (IsValid(MessengerComponent))
	{
		UWorld* world = MessengerComponent->GetWorld();

		if (IsValid(world))
		{
			EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
			FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);

			messagesCollections.PendingAddMessengerComponent.AddUnique(MessengerComponent);
			messagesCollections.PendingRemoveMessengerComponent.Remove(MessengerComponent);

			MessagesCollectionsByWorld.Add(worldType, messagesCollections);

			//world->GetTimerManager().SetTimer(TimerHandle, this, &UMessageSystemSubsystem::ProcessPendingMessageComponentsAddedRemoved, 0.1, false);
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UMessageSystemSubsystem::ProcessPendingMessageComponentsAddedRemoved, worldType);
			world->GetTimerManager().SetTimer(UMessageSystemSubsystem::ProcessPending_TimerHandle, TimerDelegate, ProcessPending_WaitTime, false);
		}
	}
}

void UMessageSystemSubsystem::QueueMessengerComponentRemoved(UMessengerComponent* MessengerComponent)
{
	if (IsValid(MessengerComponent))
	{
		UWorld* world = MessengerComponent->GetWorld();

		if (IsValid(world))
		{
			EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
			FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);

			messagesCollections.PendingRemoveMessengerComponent.AddUnique(MessengerComponent);
			messagesCollections.PendingAddMessengerComponent.Remove(MessengerComponent);

			MessagesCollectionsByWorld.Add(worldType, messagesCollections);

			//world->GetTimerManager().SetTimer(TimerHandle, this, &UMessageSystemSubsystem::ProcessPendingMessageComponentsAddedRemoved, 0.1, false);
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UMessageSystemSubsystem::ProcessPendingMessageComponentsAddedRemoved, worldType);
			world->GetTimerManager().SetTimer(UMessageSystemSubsystem::ProcessPending_TimerHandle, TimerDelegate, ProcessPending_WaitTime, false);
		}
	}
}

void UMessageSystemSubsystem::ProcessPendingMessageComponentsAddedRemoved(EWorldTypeEnum worldType)
{
	// Dragging actor in editor causes ~3 component registration + unregistrations per tick
	// This lags hard, and its for no good, we only care about the end state of a component existing or not.
	// So we wait until there's been no added or removed components for a period of time before updating.

	FMessagesCollectionsStruct* messagesCollections = MessagesCollectionsByWorld.Find(worldType);

	if (messagesCollections)
	{
		for (int i = messagesCollections->PendingRemoveMessengerComponent.Num() - 1; i >= 0; i--)
		{
			MessengerComponentRemoved(messagesCollections->PendingRemoveMessengerComponent[i].Get());
			messagesCollections->PendingRemoveMessengerComponent.RemoveAt(i);
		}

		for (int i = messagesCollections->PendingAddMessengerComponent.Num() - 1; i >= 0; i--)
		{
			MessengerComponentAdded(messagesCollections->PendingAddMessengerComponent[i].Get());
			messagesCollections->PendingAddMessengerComponent.RemoveAt(i);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Called: ProcessPendingMessageComponentsAddedRemoved"));
}

void UMessageSystemSubsystem::MessengerComponentAdded(UMessengerComponent* MessengerComponent)
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
						bool local_GiveMessagesNewID = false;
						FMessageStruct* existingMessage = messagesCollections.AllMessages.Find(MessengerComponent->MessageEvents[i].ID);
						if (existingMessage && existingMessage->SendingComponent != MessengerComponent)
						{
							local_GiveMessagesNewID = true;
						}

						
						if (local_GiveMessagesNewID // The actor may have been duplicated, and its messages will be pointing to the previous senders, we need to update those.
							//|| GiveMessagesNewID 
							|| !MessengerComponent->MessageEvents[i].ID.IsValid() // If this has a bad Guid, lets generate a new one. This could happen if someone added the event manually to the component.
							)
						{
							MessengerComponent->MessageEvents[i].ID = FGuid::NewGuid(); // bbbb <-- Annette typed those
						}

						// Add to cache.
						AddMessage(MessengerComponent->MessageEvents[i], MessengerComponent, false); // Don't broadcast, we'll handle that.
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

void UMessageSystemSubsystem::MessengerComponentUpdated(UMessengerComponent* MessengerComponent)
{
	MessengerComponentAdded(MessengerComponent);
	//if (IsValid(MessengerComponent))
	//{
	//	UWorld* world = MessengerComponent->GetWorld();
	//	if (IsValid(world))
	//	{
	//		EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
	//		FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);
	//
	//		//Only do this once, the first time the component is modified. This will also happen in a newly duplicated actor/component
	//		int index = messagesCollections.AllMessengerComponents.Find(MessengerComponent);
	//		if (index < 0)
	//		{
	//			if (MessengerComponent->MessageEvents.Num() > 0)
	//			{
	//				for (int i = 0; i < MessengerComponent->MessageEvents.Num(); i++)
	//				{
	//					//// The actor may have been duplicated, and its messages will be pointing to the previous senders, we need to update those.
	//					//if (MessengerComponent->MessageEvents[i].SendingComponent != MessengerComponent)
	//					//{
	//					//	MessengerComponent->MessageEvents[i].SendingComponent = MessengerComponent;
	//					//
	//					//	// This component/actor was likely copied which is why the sender didn't match.
	//					//	// We will get a new ID for our message, so it won't have the same ID as its copy-source.
	//					//	MessengerComponent->MessageEvents[i].ID = FGuid::NewGuid();
	//					//}
	//
	//					// If this has a bad Guid, lets generate a new one.
	//					// This could happen if someone added the event manually to the component.
	//					if (!MessengerComponent->MessageEvents[i].ID.IsValid())
	//					{
	//						MessengerComponent->MessageEvents[i].ID = FGuid::NewGuid(); // bbbb <-- Annette typed those
	//					}
	//
	//					// Add to cache.
	//					AddMessage(MessengerComponent->MessageEvents[i], MessengerComponent, false); // Don't broadcast, we'll handle that.
	//				}
	//			}
	//			else // If we have no messages to add, make sure we at-least track the component. (Normally this would happen in the AddMessage)
	//			{
	//				messagesCollections.AllMessengerComponents.AddUnique(MessengerComponent);
	//
	//				// Ensure to commit changes made here
	//				MessagesCollectionsByWorld.Add(worldType, messagesCollections);
	//			}
	//
	//			OnMessengerComponentUpdated.Broadcast(MessengerComponent);
	//		}
	//	}
	//}
}

void UMessageSystemSubsystem::MessengerComponentRemoved(UMessengerComponent* MessengerComponent)
{
	if (IsValid(MessengerComponent))
	{
		UWorld* world = MessengerComponent->GetWorld();
		if (IsValid(world))
		{
			EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
			if (FMessagesCollectionsStruct* messagesCollections = MessagesCollectionsByWorld.Find(worldType))
			{
				// Remove these messages. Reverse because this affects the array we are iterating over
				for (int i = MessengerComponent->MessageEvents.Num() - 1; i >= 0; i--)
				{
					RemoveMessage(MessengerComponent->MessageEvents[i], MessengerComponent, false); // Don't broadcast, we'll handle that.
				}

				messagesCollections->AllMessengerComponents.Remove(MessengerComponent); //TODO: Do we want to remove this, or keep it around for fun?
				FMessagesCollectionsStruct messagesCollectionsValue = *messagesCollections;
				MessagesCollectionsByWorld.Add(worldType, messagesCollectionsValue);
			}
		}
	}

	OnMessengerComponentRemoved.Broadcast(MessengerComponent);
}

void UMessageSystemSubsystem::AddMessage(FMessageStruct Message, UMessengerComponent* MessengerComponent, bool BroadcastUpdate)
{
	const UMessengerComponent* messengerComponent = MessengerComponent ? MessengerComponent : Message.SendingComponent;
	if (IsValid(messengerComponent)) // This check may not be nessesary
	{
		UWorld* world = messengerComponent->GetWorld();
		if (IsValid(world))
		{
			EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
			FMessagesCollectionsStruct messagesCollections = MessagesCollectionsByWorld.FindOrAdd(worldType);

			// Cache the sending component internally. We want to know this in the menu, but can't store
			// it in the struct that is on the actor because it creates a cyclical reference which is 
			// annoying when wanting to delete the actor.
			// But we can cache it here.
			Message.SendingComponent = MessengerComponent;

			messagesCollections.AllMessengerComponents.AddUnique(messengerComponent);
			messagesCollections.AllMessages.Add(Message.ID, Message);

			FGuidArrayStruct messagesBySender = messagesCollections.AllMessagesBySender.FindOrAdd(messengerComponent);
			messagesBySender.Array.AddUnique(Message.ID);

			FGuidArrayStruct messagesByReceivingActor = messagesCollections.AllMessagesByReceivingActor.FindOrAdd(Message.TargetActor);
			messagesByReceivingActor.Array.AddUnique(Message.ID);

			// Commit Struct Updates
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

void UMessageSystemSubsystem::RemoveMessage(FMessageStruct Message, UMessengerComponent* MessengerComponent, bool BroadcastUpdate)
{
	UMessengerComponent* messengerComponent = MessengerComponent ? MessengerComponent : Message.SendingComponent;// Using this to get the correct world
	if (IsValid(messengerComponent))
	{
		UWorld* world = messengerComponent->GetWorld();
		if (IsValid(world))
		{
			EWorldTypeEnum worldType = ToWorldTypeEnum(world->WorldType);
			if (FMessagesCollectionsStruct* messagesCollections = MessagesCollectionsByWorld.Find(worldType))
			{
				if (FMessageStruct* cachedMessage = messagesCollections->AllMessages.Find(Message.ID))
				{
					if (IsValid(MessengerComponent) && MessengerComponent != cachedMessage->SendingComponent)
					{
						// If the component that was sent in was valid, and not the same as the cached sender, then we shouldn't remove this message.
						// This can happen when the component is duplicated, and it would try to unregister the template's components.
						return;
					}

					// This could be updated if MessengerComponent sent in null, and the message we looked up had a different component than the struct that was passed in.
					messengerComponent = MessengerComponent ? MessengerComponent : cachedMessage->SendingComponent;

					// Remove this message from AllMessagesBySender cache
					if (FGuidArrayStruct* messagesBySender = messagesCollections->AllMessagesBySender.Find(messengerComponent))
					{
						messagesBySender->Array.Remove(Message.ID);

						// if that was the last one, and the list is now empty, we can remove the whole collection
						if (messagesBySender->Array.Num() == 0)
						{
							messagesCollections->AllMessagesBySender.Remove(messengerComponent);
						}
						else
						{
							// Commit Struct Updates (Do I need to? This was a pointer.)
							messagesCollections->AllMessagesBySender.Add(messengerComponent, *messagesBySender);
						}
					}

					// Remove this message from AllMessagesByReceivingActor cache
					if (FGuidArrayStruct* messagesByReceivingActor = messagesCollections->AllMessagesByReceivingActor.Find(cachedMessage->TargetActor))
					{
						messagesByReceivingActor->Array.Remove(Message.ID);

						// if that was the last one, and the list is now empty, we can remove the whole collection
						if (messagesByReceivingActor->Array.Num() == 0)
						{
							messagesCollections->AllMessagesByReceivingActor.Remove(cachedMessage->TargetActor);
						}
						else
						{
							// Commit Struct Updates (Do I need to? This was a pointer.)
							messagesCollections->AllMessagesByReceivingActor.Add(cachedMessage->TargetActor, *messagesByReceivingActor);
						}
					}
				}

				messagesCollections->AllMessages.Remove(Message.ID);

				FMessagesCollectionsStruct messagesCollectionsValue = *messagesCollections;

				// Commit Struct Updates (Do I need to? This was a pointer.)
				MessagesCollectionsByWorld.Add(worldType, messagesCollectionsValue);
			}

			if (BroadcastUpdate)
			{
				OnMessengerComponentUpdated.Broadcast(messengerComponent);
			}
		}
	}
}

void UMessageSystemSubsystem::UpdateMessage(FMessageStruct Message, UMessengerComponent* MessengerComponent, bool BroadcastUpdate)
{
	const UMessengerComponent* messengerComponent = MessengerComponent ? MessengerComponent : Message.SendingComponent;
	if (IsValid(messengerComponent))
	{
		RemoveMessage(Message, MessengerComponent, false);
		AddMessage(Message, MessengerComponent, false);

		if (BroadcastUpdate)
		{
			OnMessengerComponentUpdated.Broadcast(messengerComponent);
		}
	}
}
