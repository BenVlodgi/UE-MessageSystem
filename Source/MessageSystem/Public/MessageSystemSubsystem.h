// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MessageSystemSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MESSAGESYSTEM_API UMessageSystemSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Allow MessageSystemComponents to register themselves here. We'll hold soft references to them.
	// The Display panel will be able to use this registry to create a known list of incomming messages
	// for components (useful because only the outgoing message is actually stored on a component).
	UFUNCTION()
	void MessengerComponentUpdated(UMessengerComponent* MessengerComponent)
	{
		// Soft reference of this component to all of its inputs
	}
	
	UFUNCTION()
	void MessengerComponentRemoved(UMessengerComponent* MessengerComponent)
	{
		// Remove all outputs from this component
	}
	
};
