// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


//#include "MessageStruct.h"
//#include "MessengerComponent.h"
struct FMessageStruct;
class UMessengerComponent;

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "MessageSystemSubsystem.generated.h"


USTRUCT(BlueprintType)
struct MESSAGESYSTEM_API FMessagesArrayStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TArray<FMessageStruct> Array;
};

USTRUCT(BlueprintType)
struct MESSAGESYSTEM_API FMessagesCollectionsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TArray<UMessengerComponent*> AllMessengerComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TMap<UMessengerComponent*, FMessagesArrayStruct> AllMessagesBySender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TMap<TSoftObjectPtr<AActor>, FMessagesArrayStruct> AllMessagesByReceivingActor;
};

/**
 * Specifies the goal/source of a UWorld object
 */
UENUM(BlueprintType)
enum class EWorldTypeEnum : uint8 {
	/** An untyped world, in most cases this will be the vestigial worlds of streamed in sub-levels */
	None,

	/** The game world */
	Game,

	/** A world being edited in the editor */
	Editor,

	/** A Play In Editor world */
	PIE,

	/** A preview world for an editor tool */
	EditorPreview,

	/** A preview world for a game */
	GamePreview,

	/** A minimal RPC world for a game */
	GameRPC,

	/** An editor world that was loaded but not currently being edited in the level editor */
	Inactive
};

/**
 * 
 */
UCLASS()
class MESSAGESYSTEM_API UMessageSystemSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TMap<TEnumAsByte<EWorldTypeEnum>, FMessagesCollectionsStruct> MessagesCollectionsByWorld;


	// Allow MessageSystemComponents to register themselves here. We'll hold soft references to them.
	// The Display panel will be able to use this registry to create a known list of incomming messages
	// for components (useful because only the outgoing message is actually stored on a component).
	UFUNCTION()
		void MessengerComponentUpdated(UMessengerComponent* MessengerComponent);

	UFUNCTION()
		void MessengerComponentRemoved(UMessengerComponent* MessengerComponent);

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
	
};
