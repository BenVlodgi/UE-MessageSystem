// Fill out your copyright notice in the Description page of Project Settings.

#include "MessengerComponent.h"

#include "DynamicFunctionLibrary.h"


// Sets default values for this component's properties
UMessengerComponent::UMessengerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

#if WITH_EDITORONLY_DATA
	UBlueprint* Blueprint = Cast<UBlueprint>(GetClass()->ClassGeneratedBy);
	if (Blueprint)
	{
		Blueprint->bRunConstructionScriptOnDrag = false; // Unsure if this has an effect. If it does its good.
	}
#endif
}

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

// TODO: Check out the UObject Interface functions for nice overridable functions



//// Called when the game starts
//void UMessengerComponent::BeginPlay()
//{
//	Super::BeginPlay();
//}
//
//// Called every frame
//void UMessengerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}


void UMessengerComponent::ReceiveMessage(FMessageStruct Message, bool bAlreadyHandledByFunction)
{
	// Log and Call dispatcher
	OnReceiveMessage.Broadcast(this, Message);
}


void UMessengerComponent::BroadcastEvent(FName TriggerEventName, FDynamicParametersStruct TriggerParameters)
{
#if WITH_EDITOR
	if (GEngine)
	{
		const FString& transactionContext = FString("Send Message");
		FText transactionDescription = INVTEXT("Send Message");
		UObject* transactionPrimaryObject = NULL;
		int transaction = GEngine->BeginTransaction(*transactionContext, transactionDescription, transactionPrimaryObject);
	}
#endif

	TArray<FMessageStruct> sendMessages;
	for (FMessageStruct message : MessageEvents)
	{
		if (message.OnTrigger == TriggerEventName)
		{
			sendMessages.Add(message);
		}
	}

	for (FMessageStruct message : sendMessages)
	{
		if (!message.TargetActor.IsValid())
			continue;

		AActor* targetActor = message.TargetActor.Get();
		if (!IsValid(targetActor)) //double checking, might not need to.
			continue;

		UActorComponent* actorComponent = targetActor->GetComponentByClass(UMessengerComponent::GetClass());
		UMessengerComponent* messengerComponent = Cast<UMessengerComponent>(actorComponent);

		if (!IsValid(messengerComponent))
			continue;

		// TODO: Fill in expected parameters for this message to the Destination.
		// Example: If destination component expects Float parameter "Duration", if we don't have it in our EventParameters, then fill it with default value.
		FDynamicParametersStruct parameters = message.EventParameters;
		for (auto& TriggerParameterKVP : TriggerParameters.Parameters)
		{
			parameters.Parameters.Add(TriggerParameterKVP);
		}

		FMessageStruct sendingMessage = FMessageStruct();
		sendingMessage.SendingComponent = this;
		sendingMessage.OnTrigger = message.OnTrigger;
		sendingMessage.TargetActor = message.TargetActor;
		sendingMessage.SendEvent = message.SendEvent;
		sendingMessage.EventParameters = parameters;
		sendingMessage.Delay = message.Delay;
		sendingMessage.bCancelIfSenderIsDestroyed = message.bCancelIfSenderIsDestroyed;

#if WITH_EDITOR
		// Transact these so they will be recognized in Undo Buffer.
		if (messengerComponent) messengerComponent->Modify();
		if (targetActor) targetActor->Modify();
#endif

		bool bHandled = false;
		//UE_LOG(LogTemp, Verbose, TEXT("BroadcastEvent: '%s. Target: %s. SendEvent: %s'"), sendingMessage.OnTrigger, targetActor->GetName(), sendingMessage.SendEvent);

		// Handle this message Via Interface
		FDynamicParametersStruct callFunctionByName_ReturnValues;
		bHandled = UDynamicFunctionLibrary::CallFunctionByName(targetActor, sendingMessage.SendEvent, parameters, callFunctionByName_ReturnValues);
		messengerComponent->ReceiveMessage(sendingMessage, bHandled);
	}

#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->EndTransaction();
	}
#endif
}

void UMessengerComponent::SendMessage(AActor* TargetActor, FName SendEvent, FDynamicParametersStruct TriggerParameters)
{
	UE_LOG(LogTemp, Warning, TEXT("Called: Send Message (Not implemented yet)"));
}

void UMessengerComponent::AddMessage(FMessageStruct& Message)
{
	Message.ID = FGuid::NewGuid();
	//Message.SendingComponent = this; // Don't set sending component until message is sent. Soft references will get in the way when trying to delete the owning actor.

	MessageEvents.Add(Message);

	UWorld* world = GetWorld(); // No world during editor startup
	if (IsValid(world) && IsValid(GEngine))
	{
		UMessageSystemSubsystem* MessageSystemSubsystem = GEngine->GetEngineSubsystem<UMessageSystemSubsystem>();
		if (IsValid(MessageSystemSubsystem))
		{
			MessageSystemSubsystem->AddMessage(Message, this);
		}
	}
}

void UMessengerComponent::RemoveMessage(const FMessageStruct Message)
{
	for (int i = MessageEvents.Num() - 1; i >= 0; i--)
	{
		if (MessageEvents[i].ID == Message.ID)
		{
			MessageEvents.RemoveAt(i);
			break;
		}
	}

	UWorld* world = GetWorld(); // No world during editor startup
	if (IsValid(world) && IsValid(GEngine))
	{
		UMessageSystemSubsystem* MessageSystemSubsystem = GEngine->GetEngineSubsystem<UMessageSystemSubsystem>();
		if (IsValid(MessageSystemSubsystem))
		{
			MessageSystemSubsystem->RemoveMessage(Message, this);
		}
	}
}

void UMessengerComponent::UpdateMessage(FMessageStruct Message)
{
	for (int i = 0; i < MessageEvents.Num(); i++)
	{
		if (MessageEvents[i].ID == Message.ID)
		{
			MessageEvents[i] = Message;
			break;
		}
	}

	UWorld* world = GetWorld(); // No world during editor startup
	if (IsValid(world) && IsValid(GEngine))
	{
		UMessageSystemSubsystem* MessageSystemSubsystem = GEngine->GetEngineSubsystem<UMessageSystemSubsystem>();
		if (IsValid(MessageSystemSubsystem))
		{
			MessageSystemSubsystem->UpdateMessage(Message, this);
		}
	}
}


//void UMessengerComponent::InitializeComponent()
//{
//	Super::InitializeComponent();
//	//UE_LOG(LogTemp, Warning, TEXT("Called: InitializeComponent"));
//}

void UMessengerComponent::OnRegister()
{
	Super::OnRegister();
	//UE_LOG(LogTemp, Warning, TEXT("Called: OnRegister"));

	TArray<FMessageStruct> messageEvents = MessageEvents;

	UWorld* world = GetWorld(); // No world during editor startup
	if (IsValid(world) && IsValid(GEngine))
	{
		UMessageSystemSubsystem* MessageSystemSubsystem = GEngine->GetEngineSubsystem<UMessageSystemSubsystem>();
		if (IsValid(MessageSystemSubsystem))
		{
			MessageSystemSubsystem->QueueMessengerComponentAdded(this);
		}
	}
}

void UMessengerComponent::OnUnregister()
{
	Super::OnUnregister();
	//UE_LOG(LogTemp, Warning, TEXT("Called: OnUnregister"));

	TArray<FMessageStruct> messageEvents = MessageEvents;

	UWorld* world = GetWorld(); // No world during editor startup
	if (IsValid(world) && IsValid(GEngine))
	{
		UMessageSystemSubsystem* MessageSystemSubsystem = GEngine->GetEngineSubsystem<UMessageSystemSubsystem>();
		if (IsValid(MessageSystemSubsystem))
		{
			MessageSystemSubsystem->QueueMessengerComponentRemoved(this);
		}
	}
}

//void UMessengerComponent::PostInitProperties()
//{
//	Super::PostInitProperties();
//	//UE_LOG(LogTemp, Warning, TEXT("Called: PostInitProperties"));
//}
//
//void UMessengerComponent::PostLoad()
//{
//	Super::PostLoad();
//	//UE_LOG(LogTemp, Warning, TEXT("Called: PostLoad"));
//}
//
//void UMessengerComponent::OnComponentCreated()
//{
//	Super::OnComponentCreated();
//	//UE_LOG(LogTemp, Warning, TEXT("Called: OnComponentCreated"));
//
//	//UWorld* world = GetWorld(); // No world during editor startup
//	//if (IsValid(world) && IsValid(GEngine))
//	//{
//	//	UMessageSystemSubsystem* MessageSystemSubsystem = GEngine->GetEngineSubsystem<UMessageSystemSubsystem>();
//	//	if (IsValid(MessageSystemSubsystem))
//	//	{
//	//		QueueMessageSystemSubsystem->MessengerComponentAdded(this);
//	//	}
//	//}
//}
//
//void UMessengerComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
//{
//	Super::OnComponentDestroyed(bDestroyingHierarchy);
//	//UE_LOG(LogTemp, Warning, TEXT("Called: OnComponentDestroyed"));
//
//
//	//UWorld* world = GetWorld(); // No world during editor startup
//	//if (IsValid(world) && IsValid(GEngine))
//	//{
//	//	UMessageSystemSubsystem* MessageSystemSubsystem = GEngine->GetEngineSubsystem<UMessageSystemSubsystem>();
//	//	if (IsValid(MessageSystemSubsystem))
//	//	{
//	//		MessageSystemSubsystem->MessengerComponentRemoved(this);
//	//	}
//	//}
//}
//
////void UMessengerComponent::DetermineUCSModifiedProperties()
////{
////	Super::DetermineUCSModifiedProperties();
////	UE_LOG(LogTemp, Warning, TEXT("Called: DetermineUCSModifiedProperties"));
////	TArray<FMessageStruct> messageEvents = MessageEvents;
////}
//
//bool UMessengerComponent::Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags)
//{
//	bool returnValue = Super::Rename(NewName, NewOuter, Flags);
//	//UE_LOG(LogTemp, Warning, TEXT("Called: Rename: NewName:'%s'"), NewName);
//	return returnValue;
//}
//
//void UMessengerComponent::PostRename(UObject* OldOuter, const FName OldName)
//{
//	Super::PostRename(OldOuter, OldName);
//	//UE_LOG(LogTemp, Warning, TEXT("Called: PostRename: OldName:'%s'"), *OldName.ToString());
//}
//
//#if WITH_EDITOR
//bool UMessengerComponent::Modify(bool bAlwaysMarkDirty)
//{
//	bool returnValue = Super::Modify(bAlwaysMarkDirty);
//	//UE_LOG(LogTemp, Warning, TEXT("Called: Modify"));
//
//	return returnValue;
//}
//
//void UMessengerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//	//UE_LOG(LogTemp, Warning, TEXT("Called: PostEditChangeProperty"));
//}
//
//void UMessengerComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeChainProperty(PropertyChangedEvent);
//	//UE_LOG(LogTemp, Warning, TEXT("Called: PostEditChangeChainProperty"));
//}
//#endif
