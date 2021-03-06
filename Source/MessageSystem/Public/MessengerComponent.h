// Copyright 2022 Dream Seed LLC.

#pragma once

#include "MessageSystemSubsystem.h"
#include "Structs/MessageStruct.h"
#include "MessageSystemBPLibrary.h"
#include "Misc/Guid.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessengerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveMessageDelegate, UMessengerComponent*, DeliveringComponent, FMessageStruct, Message);

UCLASS(Blueprintable, ClassGroup=(MessageSystem), meta=(BlueprintSpawnableComponent))
class MESSAGESYSTEM_API UMessengerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Broadcast these messages when event is triggered.
	// Note: Sending Component value is not set while in Sending Component's array. It is filled in when used externally, like when message is sent. This is becauses soft references will get in the way when trying to delete the owning actor.
	// TODO: Replace Message list with TMap.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System")
	TArray<FMessageStruct> MessageEvents; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Config")
	FName DefaultBroadcastEventName;

	// Events this actor broadcasts and parameters those events may send.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Config")
	TMap<FName, FDynamicParametersStruct> BroadcastEventDefinitions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message System|Config")
	TMap<FName, FDynamicParametersStruct> CustomInboundEventDefinitions;

	UPROPERTY(BlueprintAssignable, Category = "MessageSystem")
	FReceiveMessageDelegate OnReceiveMessage;

protected:
	//// Called when the game starts
	//virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UMessengerComponent();

	//// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 
	* Receiving the Message on our MessengerComponent.  If the message's SendEvent is a function name, then it has been called (or attempted) already.
	* 
	* @param Message Message sent from an external source.
	* @param bAlreadyHandledByFunction This message has been sent as a function call, and was executed successfully.
	*/
	UFUNCTION(BlueprintCallable, Category = "MessageSystem")
	void ReceiveMessage(FMessageStruct Message, bool bAlreadyHandledByFunction = false);

	UFUNCTION(BlueprintCallable, Category = "MessageSystem")
	void BroadcastEvent(FName TriggerEventName, FDynamicParametersStruct TriggerParameters);

	UFUNCTION(BlueprintCallable, Category = "MessageSystem")
	void SendMessage(AActor* TargetActor, FName SendEvent, FDynamicParametersStruct TriggerParameters);

	UFUNCTION(BlueprintCallable, Category = "MessageSystem|Manage")
	void AddMessage(FMessageStruct& Message);

	UFUNCTION(BlueprintCallable, Category = "MessageSystem|Manage")
	void RemoveMessage(const FMessageStruct Message);

	UFUNCTION(BlueprintCallable, Category = "MessageSystem|Manage")
	void UpdateMessage(FMessageStruct Message);
	
	//UFUNCTION(BlueprintCallable, Category = "MessageSystem|Manage")
	//void RemoveMessageByID(FGuid ID);

	UFUNCTION(BlueprintCallable, Category = "MessageSystem|Manage")
	void LookupMessage(FGuid ID, bool& bFound, FMessageStruct& Message);



// TODO, implement some registering with the sub system.
// Can't happen during construct, causes crash on editor startup
// try:
// - OnRegister
// - InitializeComponent
// - PostInitProperties
// - PostLoad
// - Modify
// - PostEditChangeProperty
// - PostEditChangeChainProperty

	// TODO: Check if this will work?
	//PostDuplicate(bool bDuplicateForPIE)


protected:
	void OnRegister() override; // Called when a component is registered, after Scene is set, but before CreateRenderState_Concurrent or OnCreatePhysicsState are called.
	void OnUnregister() override; // Called when a component is unregistered. Called after DestroyRenderState_Concurrent and OnDestroyPhysicsState are called.

//public:
//	void InitializeComponent() override;
//	void PostInitProperties() override; // Called after the C++ constructor and after the properties have been initialized, including those loaded from config. // This is called first When added or begin play happens. 
//	void PostLoad() override;
//	void OnComponentCreated() override;
//	void OnComponentDestroyed(bool bDestroyingHierarchy) override;
//
//	//void DetermineUCSModifiedProperties() override; // Initializes the list of properties that are modified by the UserConstructionScript
//	bool Rename(const TCHAR* NewName = NULL, UObject* NewOuter = NULL, ERenameFlags Flags = REN_None) override;
//	void PostRename(UObject* OldOuter, const FName OldName) override;
//
//#if WITH_EDITOR
//	bool Modify(bool bAlwaysMarkDirty = true) override; // That the object will be modified. If we are currently recording into the transaction buffer (undo/redo), save a copy of this object into the buffer and marks the package as needing to be saved.// This is called second When added or begin play happens. 
//	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//	void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
//#endif

};
