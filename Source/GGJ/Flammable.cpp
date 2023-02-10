// Fill out your copyright notice in the Description page of Project Settings.


#include "Flammable.h"

#include "FireComponent.h"

bool UFlammable::DefaultIsFlaming(UObject* object)
{
	if (AActor* actor = Cast<AActor>(object))
	{
		return actor->GetComponentByClass(UFireComponent::StaticClass()) != nullptr;
	}

	return false;
}

// Add default functionality here for any IFlammable functions that are not pure virtual.
bool IFlammable::IsFlaming_Implementation() const
{
	return false;
}
