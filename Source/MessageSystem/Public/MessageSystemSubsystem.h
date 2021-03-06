// Copyright 2022 Dream Seed LLC.

#pragma once

//#include "MessengerComponent.h"
class UMessengerComponent;
#include "Enums/WorldTypeEnum.h"
#include "Structs/GuidArrayStruct.h"
#include "Structs/MessagesCollectionsStruct.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Subsystems/EngineSubsystem.h"

#include "MessageSystemSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessengerComponentDelegate, const UMessengerComponent*, MessengerComponent);

UCLASS()
class MESSAGESYSTEM_API UMessageSystemSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	/** Implement this for initialization of instances of the system */
	void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
#if WITH_EDITOR
	void OnLevelActorAdded(AActor* Actor);
	void OnLevelActorDeleted(AActor* Actor);
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Organization")
	TMap<TEnumAsByte<EWorldTypeEnum>, FMessagesCollectionsStruct> MessagesCollectionsByWorld;

	UPROPERTY(BlueprintAssignable, Category = "MessageSystem")
	FMessengerComponentDelegate OnMessengerComponentUpdated;

	UPROPERTY(BlueprintAssignable, Category = "MessageSystem")
	FMessengerComponentDelegate OnMessengerComponentRemoved;

	float ProcessPending_WaitTime = 0.1;
	FTimerHandle ProcessPending_TimerHandle;


	// Allow MessageSystemComponents to register themselves here. We'll hold soft references to them.
	// The Display panel will be able to use this registry to create a known list of incomming messages
	// for components (useful because only the outgoing message is actually stored on a component).



	UFUNCTION()
	void QueueMessengerComponentAdded(UMessengerComponent* MessengerComponent);

	UFUNCTION()
	void QueueMessengerComponentRemoved(UMessengerComponent* MessengerComponent);

	UFUNCTION()
	void ProcessPendingMessageComponentsAddedRemoved(EWorldTypeEnum worldType);

	UFUNCTION()
	void MessengerComponentAdded(UMessengerComponent* MessengerComponent);

	UFUNCTION()
	void MessengerComponentUpdated(UMessengerComponent* MessengerComponent);

	UFUNCTION()
	void MessengerComponentRemoved(UMessengerComponent* MessengerComponent);

	UFUNCTION()
	void AddMessage(FMessageStruct Message, UMessengerComponent* MessengerComponent = NULL, bool BroadcastUpdate = true, int SpecificMessageBySenderIndex = INDEX_NONE);

	UFUNCTION()
	void RemoveMessage(FMessageStruct Message, UMessengerComponent* MessengerComponent = NULL, bool BroadcastUpdate = true);

	UFUNCTION()
	void UpdateMessage(FMessageStruct Message, UMessengerComponent* MessengerComponent = NULL, bool BroadcastUpdate = true);

	UFUNCTION()
	int GetMessageBySenderIndex(FMessageStruct Message, UMessengerComponent* MessengerComponent = NULL);

	EWorldTypeEnum ToWorldTypeEnum(TEnumAsByte<EWorldType::Type> WorldType)
	{
		switch (WorldType)
		{
			case EWorldType::None: return EWorldTypeEnum::None;
			case EWorldType::Game: return EWorldTypeEnum::Game;
			case EWorldType::Editor: return EWorldTypeEnum::Editor;
			case EWorldType::PIE: return EWorldTypeEnum::PIE;
			case EWorldType::EditorPreview: return EWorldTypeEnum::EditorPreview;
			case EWorldType::GamePreview: return EWorldTypeEnum::GamePreview;
			case EWorldType::GameRPC: return EWorldTypeEnum::GameRPC;
			case EWorldType::Inactive: return EWorldTypeEnum::Inactive; 
		}
		return EWorldTypeEnum::None; // This will never happen unless a new World type is added to the engine, but not here.
	}



	UFUNCTION(BlueprintCallable, Category = "MessageSystem", meta = (DisplayName = "Lookup Message", KeyWords = "find"))
	void LookupMessage(bool& bFound, FMessageStruct& Message, FGuid ID, EWorldTypeEnum WorldType = EWorldTypeEnum::Editor);
	
};
