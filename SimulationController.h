// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include <cstdlib>
#include <ctime>
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyCube.h"
#include "HAL/RunnableThread.h"
#include "HAL/Runnable.h"
#include <vector>
#include "Misc/DateTime.h"
#include "SimulationController.generated.h"

class CubeColor {
public:
	CubeColor();
	CubeColor(float, float, float);
	float R;
	float G;
	float B;
};

class ColorTab {
public:
	ColorTab();
	ColorTab(int);
	CubeColor* tab;
};

class Coordinates {
public:
	Coordinates();
	Coordinates(int, int, int);
	int x;
	int y;
	int z;
};

UCLASS()
class SIMPLEGRAINGROWTH_API ASimulationController : public AActor, public FRunnable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimulationController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//PROPERTY IN UNREAL TO CHOSE ACTOR TO SPAWN
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> toSpawn;

	//INITIATE VARIABLES AND FUNCTIONS
	UFUNCTION(BlueprintCallable)
	void initiate(int grainsTmp, int sizeTmp, float scale);
	int16 grains;
	int16 size;
	float scale;
	void setGrains(int grainsTmp);
	void setSize(int sizeTmp);
	void setScale(float scaleTmp);

	//SETUP
	UFUNCTION(BlueprintCallable)
	void setup();

	//COMPUTING STEP
	UFUNCTION(BlueprintCallable)
	void computeStep();
	UFUNCTION(BlueprintCallable)
	void computeNonOptimal();
	bool checkIfDone();
	UFUNCTION(BlueprintCallable)
	void computeOptimal(int inclusions, int radius, bool circle, bool boundaries, int chance);
	void drawCubes();
	UFUNCTION(BlueprintCallable)
	void reComputeOptimal(int grainsTmp, int inclusions, int radius, bool circle, bool boundaries, int chance);
	UFUNCTION(BlueprintCallable)
	void dualPhase(int grainsTmp, int inclusions, int radius, bool circle, bool boundaries, int chance);

	//INCLUSIONS
	UFUNCTION(BlueprintCallable)
	void beforeSimulationInclusion(int inclusions, int radius, bool circle);
	void afterSimulationInclusion(int inclusions, int radius, bool circle);
	void squareInclusin(int, int, int, int);
	void circleInclusion(int, int, int, int);

	//GRAINS BOUNDARIES
	UFUNCTION(BlueprintCallable)
	void grainsBoundaries();
	UFUNCTION(BlueprintCallable)
	void printPercentageOfBoundaries();

	//RESETING AND DELETING
	UFUNCTION(BlueprintCallable)
	void reset();
	UFUNCTION(BlueprintCallable)
	void deleteSome();
	UFUNCTION(BlueprintCallable)
	void resetIdVector();

	//TIME MENAGMENT
	FDateTime returnTime();
	int ComputeTime(FDateTime, FDateTime);

	//FUNCION SPAWNING CUBE IN ENGINE
	UFUNCTION(BlueprintCallable)
	AActor* spawnCube(int x, int y, int z, int index);

	//CHANGING VIEW ON SCROLL
	UFUNCTION(BlueprintCallable)
	void scrollUp();
	UFUNCTION(BlueprintCallable)
	void scrollDown();
	int height;

	//MONTE CARLO
	UFUNCTION(BlueprintCallable)
	void monteCarloSetup(int grainsTmp, int sizeTmp, float scale);
	UFUNCTION(BlueprintCallable)
	void computeMonteCarlo(int n);
	void computeMonteCarloStep();
	UFUNCTION(BlueprintCallable)
	void dualphaseMonteCarlo(int grainsTmp);
	UFUNCTION(BlueprintCallable)
	void computeMonteCarloDualphase(int n);
	void computeMonteCarloStepDualphase();

	//RECRYSTALISATION
	UFUNCTION(BlueprintCallable)
	void redistributeEnergyHomogenous();
	UFUNCTION(BlueprintCallable)
	void redistributeEnergyHeterogenous();
	void resetEnergy();
	UFUNCTION(BlueprintCallable)
	void SRX(int n, int timesNewNucleons, int step, bool boundaries);
	UFUNCTION(BlueprintCallable)
	void SRXinitiation(int nucleons, bool boundaries);
	UFUNCTION(BlueprintCallable)
	void showEnergy();
	UFUNCTION(BlueprintCallable)
	void showCubes();

	//TABS
	ColorTab colorTable;
	int*** tab;
	int*** tabOld;
	int* mostTab;
	int max;
	int maxId;
	int newId;
	void rewriteTab();
	void clearMostTab();
	int checkMost(int x, int y, int z);
	int checkMostMonteCarlo(int x, int y, int z);
	int checkMostRecrystalisation(int x, int y, int z);
	std::vector<AMyCube*> cubeVector;
	double*** energyTab;

	//Multithreading
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	int a;
	FRunnableThread* thread1;
	FRunnableThread* thread2;
	UFUNCTION(BlueprintCallable)
	void callThread();
};


