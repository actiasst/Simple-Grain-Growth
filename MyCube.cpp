// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCube.h"

// Sets default values
AMyCube::AMyCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

std::vector<int> AMyCube::idVector;

void AMyCube::NotifyActorOnClicked(FKey ButtonPressed)
{
	FLinearColor color(0, 0, 0);
	bool condition = true;
	for (int i = 0; i < idVector.size(); i++)
		if (idVector[i] == id)
			condition = false;
	if (condition) {
		idVector.push_back(id);
		UKismetSystemLibrary::PrintString(GetWorld(), FString("Added ") + FString::FromInt(id) + FString(" ID"), true, false, color, 2);
	}
}
