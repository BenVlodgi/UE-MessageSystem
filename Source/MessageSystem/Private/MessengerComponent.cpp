// Fill out your copyright notice in the Description page of Project Settings.


#include "MessengerComponent.h"



// Sets default values for this component's properties
UMessengerComponent::UMessengerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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



// Called when the game starts
void UMessengerComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UMessengerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UMessengerComponent::ReceiveMessage(FMessageStruct Message)
{
	// Log and Call dispatcher
	OnReceiveMessage.Broadcast(this, Message);
}

void UMessengerComponent::BroadcastEvent(FName TriggerEventName, FMessageParametersStruct TriggerParameters)
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
		if (!message.TargetActorSoft.IsValid())
			continue;

		AActor* targetActor = message.TargetActorSoft.Get();
		if (!IsValid(targetActor)) //double checking, might not need to.
			continue;

		UActorComponent* actorComponent = targetActor->GetComponentByClass(UMessengerComponent::GetClass());
		UMessengerComponent* messengerComponent = Cast<UMessengerComponent>(actorComponent);

		if (!IsValid(messengerComponent))
			continue;

		// TODO: Fill in expected parameters for this message to the Destination.
		// Example: If destination component expects Float parameter "Duration", if we don't have it in our EventParameters, then fill it with default value.
		FMessageParametersStruct parameters = message.EventParameters;
		for (auto& TriggerParameterKVP : TriggerParameters.Parameters)
		{
			parameters.Parameters.Add(TriggerParameterKVP);
		}

		FMessageStruct sendingMessage = FMessageStruct();
		sendingMessage.SendingActor = this->GetOwner();
		sendingMessage.SendingComponent = this;
		sendingMessage.OnTrigger = message.OnTrigger;
		sendingMessage.TargetActorSoft = message.TargetActorSoft;
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

		// TODO: Try to handle this message Via Interface
		FMessageParametersStruct callWithInterfaceSuccess_ReturnValues;
		bHandled = UMessageSystemBPLibrary::CallFunctionByNameWithArguments(targetActor, sendingMessage.SendEvent, parameters, callWithInterfaceSuccess_ReturnValues);

		if (!bHandled || true)
		{
			messengerComponent->ReceiveMessage(sendingMessage);
		}
	} 

#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->EndTransaction();
	}
#endif
}

void UMessengerComponent::SendMessage(AActor* TargetActor, FName SendEvent, FMessageParametersStruct TriggerParameters)
{
	UE_LOG(LogTemp, Warning, TEXT("Called: Send Message (Not implemented yet)"));
}


void UMessengerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	UE_LOG(LogTemp, Warning, TEXT("Called: InitializeComponent"));
}


void UMessengerComponent::OnRegister()
{
	Super::OnRegister();
	UE_LOG(LogTemp, Warning, TEXT("Called: OnRegister"));
}

void UMessengerComponent::PostInitProperties()
{
	Super::PostInitProperties();
	UE_LOG(LogTemp, Warning, TEXT("Called: PostInitProperties"));
}

void UMessengerComponent::PostLoad()
{
	Super::PostLoad();
	UE_LOG(LogTemp, Warning, TEXT("Called: PostLoad"));
}


#if WITH_EDITOR
bool UMessengerComponent::Modify(bool bAlwaysMarkDirty)
{
	bool returnValue = Super::Modify(bAlwaysMarkDirty);
	UE_LOG(LogTemp, Warning, TEXT("Called: Modify"));

	// Update Editor registry of all these components

	//UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	//if (IsValid(GameInstance))
	//{
	//	UMessageSystemSubsystem* MessageSystemSubsystem = UGameInstance::GetSubsystem<UMessageSystemSubsystem>(GameInstance);
	//	if (IsValid(MessageSystemSubsystem))
	//	{
	//		MessageSystemSubsystem->MessengerComponentUpdated(this);
	//	}
	//}

	return returnValue;
}

void UMessengerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UE_LOG(LogTemp, Warning, TEXT("Called: PostEditChangeProperty"));
}

void UMessengerComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	UE_LOG(LogTemp, Warning, TEXT("Called: PostEditChangeChainProperty"));
}
#endif





