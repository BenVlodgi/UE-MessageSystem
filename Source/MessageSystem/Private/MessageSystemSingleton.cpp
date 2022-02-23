// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageSystemSingleton.h"


static UMessageSystemSingleton* SelfInstance;

UMessageSystemSingleton* UMessageSystemSingleton::GetMessageSystemInstance()
{
	if (!IsValid(SelfInstance))
	{
		SelfInstance = NewObject<UMessageSystemSingleton>();
	}
	return SelfInstance;
	//return nullptr;
}