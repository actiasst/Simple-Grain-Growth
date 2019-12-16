// Fill out your copyright notice in the Description page of Project Settings.

#include "SimulationController.h"

// Sets default values
ASimulationController::ASimulationController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	grains = 0;
	size = 0;
}

// Called when the game starts or when spawned
void ASimulationController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASimulationController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

CubeColor::CubeColor()
{
	R = 1;
	G = 1;
	B = 1;
}

CubeColor::CubeColor(float R, float G, float B)
{
	this->R = R;
	this->G = G;
	this->B = B;
}

ColorTab::ColorTab() {
	tab = NULL;
}

ColorTab::ColorTab(int grains) {
	tab = new CubeColor[grains + 12];
	tab[0] = CubeColor(1, 1, 1);
	tab[1] = CubeColor(0, 0, 0);
	tab[grains + 2] = CubeColor(1, 0, 0);
	tab[grains + 3] = CubeColor(1, 0, 0.1);
	tab[grains + 4] = CubeColor(1, 0, 0.2);
	tab[grains + 5] = CubeColor(1, 0, 0.3);
	tab[grains + 6] = CubeColor(1, 0, 0.4);
	tab[grains + 7] = CubeColor(1, 0, 0.5);
	tab[grains + 8] = CubeColor(1, 0, 0.6);
	tab[grains + 9] = CubeColor(1, 0, 0.7);
	tab[grains + 10] = CubeColor(1, 0, 0.8);
	tab[grains + 11] = CubeColor(1, 0, 0.9);
	for (int i = 2; i < grains + 2; i++) {
		tab[i].R = (float)(rand() % 101) / 100.;
		tab[i].G = (float)(rand() % 101) / 100.;
		tab[i].B = (float)(rand() % 101) / 100.;
	}
}

AActor* ASimulationController::spawnCube(int x, int y, int z, int index)
{
	FLinearColor color(0, 0, 0);
	//UKismetSystemLibrary::PrintString(GetWorld(),(FString)("xD"), true, false, color, 2);
	AActor* spawnedActor = NULL;
	if (toSpawn) {
		UWorld* world = GetWorld();
		if (world) {
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;
			FRotator rotator = rotator.ZeroRotator;
			FVector spawnLocation;
			spawnLocation.Set(x * 100 * scale, y * 100 * scale, z * 100 * scale);
			spawnedActor = world->SpawnActor<AActor>(toSpawn, spawnLocation, rotator, spawnParams);
			TArray<UStaticMeshComponent*> components;
			spawnedActor->GetComponents<UStaticMeshComponent>(components);
			UStaticMeshComponent* myStaticMeshComponent = components[0];
			UMaterialInstanceDynamic* myMaterial = UMaterialInstanceDynamic::Create(myStaticMeshComponent->GetMaterial(0), this);
			FLinearColor newColor = FLinearColor(colorTable.tab[index].R, colorTable.tab[index].G, colorTable.tab[index].B, 1.0f);
			myMaterial->SetVectorParameterValue("Color", newColor);
			myStaticMeshComponent->SetMaterial(0, myMaterial);
			myStaticMeshComponent->SetRelativeScale3D(FVector(scale, scale, scale));
			dynamic_cast<AMyCube*>(spawnedActor)->x = x;
			dynamic_cast<AMyCube*>(spawnedActor)->y = y;
			dynamic_cast<AMyCube*>(spawnedActor)->z = z;
			dynamic_cast<AMyCube*>(spawnedActor)->id = index;
			cubeVector.push_back(dynamic_cast<AMyCube*>(spawnedActor));
			}
	}
	return spawnedActor;
}

void ASimulationController::scrollUp()
{
	height++;
	if (height > size - 1)
		height = size - 1;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			spawnCube(i, j, height - 1, tab[i][j][height - 1]);
}

void ASimulationController::scrollDown()
{
	height--;
	if (height < 2)
		height = 2;
	for (int i = cubeVector.size() - 1; i > -1; i--) {
		if (cubeVector[i]->z == height) {
			cubeVector[i]->Destroy();
			cubeVector.erase(cubeVector.begin() + i);
		}
	}
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			spawnCube(i, j, height-1, tab[i][j][height-1]);
}

void ASimulationController::monteCarloSetup(int grainsTmp, int sizeTmp, float scale)
{
	initiate(grainsTmp, sizeTmp, scale);
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				tab[i][j][k] = (rand() % grainsTmp) + 2;
	drawCubes();
}

void ASimulationController::computeMonteCarlo(int n)
{
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	for (int i = 0; i < n; i++)
		computeMonteCarloStep();
	drawCubes();
}

void ASimulationController::computeMonteCarloStep()
{
	FLinearColor color(0, 0, 0);
	//UKismetSystemLibrary::PrintString(GetWorld(),(FString)("xD"), true, false, color, 2);
	std::vector<Coordinates> vectorCoordinates;
	int counter = 0;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++) {
				vectorCoordinates.insert(vectorCoordinates.begin() + counter, Coordinates(i, j, k));
				counter++;
			}
	int index;
	int Eprev;
	int Enow;
	counter = 0;
	int idTmp;
	bool flag = true;
	while (vectorCoordinates.size()) {
		index = rand() % vectorCoordinates.size();
		idTmp = tab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z];
		for (int i = vectorCoordinates[index].x - 1; i < vectorCoordinates[index].x + 2; i++)
			for (int j = vectorCoordinates[index].y - 1; j < vectorCoordinates[index].y + 2; j++)
				for (int k = vectorCoordinates[index].z - 1; k < vectorCoordinates[index].z + 2; k++)
					if (tab[i][j][k] != 0 && tab[i][j][k] != idTmp)
						counter++;
		Eprev = counter;
		counter = 0;
		while (flag) {
			idTmp = (rand() % grains) + 2;
			for (int i = vectorCoordinates[index].x - 1; i < vectorCoordinates[index].x + 2; i++)
				for (int j = vectorCoordinates[index].y - 1; j < vectorCoordinates[index].y + 2; j++)
					for (int k = vectorCoordinates[index].z - 1; k < vectorCoordinates[index].z + 2; k++)
						if (tab[i][j][k] != 0 && tab[i][j][k] != idTmp)
							counter++;
			Enow = counter;
			counter = 0;
			if (Enow - Eprev <= 0) {
				tab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z] = idTmp;
				vectorCoordinates.erase(vectorCoordinates.begin() + index);
				flag = false;
			}
		}
		flag = true;
	}
}

void ASimulationController::dualphaseMonteCarlo(int grainsTmp)
{
	CubeColor* tmp = new CubeColor[grainsTmp + 12];
	tmp[2] = colorTable.tab[cubeVector[0]->idVector[0]];
	tmp[0] = CubeColor(1, 1, 1);
	tmp[1] = CubeColor(0, 0, 0);
	tmp[grains + 2] = CubeColor(1, 0, 0);
	tmp[grains + 3] = CubeColor(1, 0, 0.1);
	tmp[grains + 4] = CubeColor(1, 0, 0.2);
	tmp[grains + 5] = CubeColor(1, 0, 0.3);
	tmp[grains + 6] = CubeColor(1, 0, 0.4);
	tmp[grains + 7] = CubeColor(1, 0, 0.5);
	tmp[grains + 8] = CubeColor(1, 0, 0.6);
	tmp[grains + 9] = CubeColor(1, 0, 0.7);
	tmp[grains + 10] = CubeColor(1, 0, 0.8);
	tmp[grains + 11] = CubeColor(1, 0, 0.9);
	setGrains(grainsTmp);
	delete[] mostTab;
	mostTab = new int[grains + 12];
	for (int i = 3; i < grains + 2; i++)
		tmp[i] = CubeColor((float)(rand() % 101) / 100., (float)(rand() % 101) / 100., (float)(rand() % 101) / 100.);
	delete[] colorTable.tab;
	colorTable.tab = tmp;

	for(int l = 0; l < cubeVector[0]->idVector.size();l++)
		for (int i = 1; i < size - 1; i++)
			for (int j = 1; j < size - 1; j++)
				for (int k = 1; k < size - 1; k++)
					if (tab[i][j][k] == cubeVector[0]->idVector[l])
						tab[i][j][k] = 2;

	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				if (tab[i][j][k] == 0)
					tab[i][j][k] = rand() % (grainsTmp - 1) + 3;
	drawCubes();
}

void ASimulationController::computeMonteCarloDualphase(int n)
{
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	for (int i = 0; i < n; i++)
		computeMonteCarloStepDualphase();
	drawCubes();
}

void ASimulationController::computeMonteCarloStepDualphase()
{
	FLinearColor color(0, 0, 0);
	//UKismetSystemLibrary::PrintString(GetWorld(),(FString)("xD"), true, false, color, 2);
	std::vector<Coordinates> vectorCoordinates;
	int counter = 0;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++) {
				if (tab[i][j][k] != 2) {
					vectorCoordinates.insert(vectorCoordinates.begin() + counter, Coordinates(i, j, k));
					counter++;
				}
			}
	int index;
	int Eprev;
	int Enow;
	counter = 0;
	int idTmp;
	bool flag = true;
	while (vectorCoordinates.size()) {
		index = rand() % vectorCoordinates.size();
		idTmp = tab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z];
		for (int i = vectorCoordinates[index].x - 1; i < vectorCoordinates[index].x + 2; i++)
			for (int j = vectorCoordinates[index].y - 1; j < vectorCoordinates[index].y + 2; j++)
				for (int k = vectorCoordinates[index].z - 1; k < vectorCoordinates[index].z + 2; k++)
					if (tab[i][j][k] != 0 && tab[i][j][k] != idTmp)
						counter++;
		Eprev = counter;
		counter = 0;
		while (flag) {
			idTmp = (rand() % grains) + 3;
			for (int i = vectorCoordinates[index].x - 1; i < vectorCoordinates[index].x + 2; i++)
				for (int j = vectorCoordinates[index].y - 1; j < vectorCoordinates[index].y + 2; j++)
					for (int k = vectorCoordinates[index].z - 1; k < vectorCoordinates[index].z + 2; k++)
						if (tab[i][j][k] != 0 && tab[i][j][k] != idTmp)
							counter++;
			Enow = counter;
			counter = 0;
			if (Enow - Eprev <= 0) {
				tab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z] = idTmp;
				vectorCoordinates.erase(vectorCoordinates.begin() + index);
				flag = false;
			}
		}
		flag = true;
	}
}

void ASimulationController::redistributeEnergyHomogenous()
{
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				energyTab[i][j][k] = 5 + ((rand() % 101) - 50) / 100.;
}

void ASimulationController::redistributeEnergyHeterogenous()
{
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				if(tab[i][j][k]==1)
					energyTab[i][j][k] = 7 + ((rand() % 141) - 70) / 100.;
				else
					energyTab[i][j][k] = 2 + ((rand() % 41) - 20) / 100.;
}

void ASimulationController::resetEnergy()
{
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				energyTab[i][j][k] = 0;
}

void ASimulationController::SRXinitiation(int nucleons, bool boundaries)
{
	bool spawned = false;
	int x, y, z;
	for (int i = 0; i < nucleons; i++) {
		while (!spawned) {
			x = (rand() % (size - 2)) + 1;
			y = (rand() % (size - 2)) + 1;
			z = (rand() % (size - 2)) + 1;
			if (boundaries) {
				if (tab[x][y][z] == 1) {
					tab[x][y][z] = grains + 2 + (rand() % 10);
					energyTab[x][y][z] = 0;
					spawned = true;
				}
			}
			else {
				tab[x][y][z] = grains + 2 + (rand() % 10);
				energyTab[x][y][z] = 0;
				spawned = true;
			}
		}
		spawned = false;
	}
}

void ASimulationController::showEnergy()
{
	int ***tabCopy = new int**[size];
	for (int i = 0; i < size; i++)
		tabCopy[i] = new int*[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			tabCopy[i][j] = new int[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				tabCopy[i][j][k] = tab[i][j][k];

	CubeColor *colorCopy = new CubeColor[grains+12];
	for (int i = 0; i < grains + 12; i++)
		colorCopy[i] = colorTable.tab[i];

	delete[] colorTable.tab;
	colorTable.tab = new CubeColor[29];
	colorTable.tab[28] = CubeColor(0,0,0);
	float step;
	step = 0.2;
	for (int i = 0; i < 4; i++)
		colorTable.tab[i] = CubeColor(0, step + step * i, 0);
	step = 0.09;
	for (int i = 4; i < 14; i++)
		colorTable.tab[i] = CubeColor(step + step * (i - 4), 0, 0);
	step = 0.06;
	for (int i = 14; i < 29; i++)
		colorTable.tab[i] = CubeColor(0, 0, step + step * (i - 14));

	step = 0.1;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++) {
				for (int l = 0; l < 4; l++)
					if (energyTab[i][j][k] > 1.79 + step * l && energyTab[i][j][k] <= 1.9 + step * l)
						tab[i][j][k] = l;
				for (int l = 0; l < 10; l++)
					if (energyTab[i][j][k] > 4.49 + step * l && energyTab[i][j][k] <= 4.6 + step * l)
						tab[i][j][k] = l + 4;
				for (int l = 0; l < 14; l++)
					if (energyTab[i][j][k] > 6.29 + step * l && energyTab[i][j][k] <= 6.4 + step * l)
						tab[i][j][k] = l + 14;
				if (energyTab[i][j][k] == 0)
					tab[i][j][k] = 28;
			}
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	drawCubes();

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				tab[i][j][k] = tabCopy[i][j][k];
	
	delete[] colorTable.tab;
	colorTable.tab = new CubeColor[grains+12];
	for (int i = 0; i < grains + 12; i++)
		colorTable.tab[i] = colorCopy[i];

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			delete[] tabCopy[i][j];
	for (int i = 0; i < size; i++)
		delete[] tabCopy[i];
	delete[] tabCopy;
	delete[] colorCopy;
}

void ASimulationController::showCubes()
{
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	drawCubes();
}

void ASimulationController::SRX(int n, int timesNewNucleons, int step, bool bundaries)
{
	std::vector<Coordinates> vectorCoordinates;
	int nucleons = step;
	int newNucleonsCounter = 0;
	int iterationsCounter = 0;
	int counter = 0;
	int index;
	double Eprev;
	double Enow;
	int idTmp;
	for (int l = 0; l < n; l++) {
		/*for (int i = 1; i < size - 1; i++)
			for (int j = 1; j < size - 1; j++)
				for (int k = 1; k < size - 1; k++)*/ for (int i = 1; i < size - 1; i++)
					for (int j = 1; j < size - 1; j++)
						for (int k = 1; k < height; k++) {
					if (tab[i][j][k] < grains + 2) {
						vectorCoordinates.insert(vectorCoordinates.begin() + counter, Coordinates(i, j, k));
						counter++;
					}
				}
		while (vectorCoordinates.size()) {
			if (iterationsCounter % 10 == 9 && newNucleonsCounter < timesNewNucleons) {
				SRXinitiation(nucleons, bundaries);
				nucleons += step;
				timesNewNucleons++;
			}
			index = rand() % vectorCoordinates.size();
			counter = 0;
			idTmp = checkMostRecrystalisation(vectorCoordinates[index].x, vectorCoordinates[index].y, vectorCoordinates[index].z);
			if (idTmp != 0) {
				for (int i = vectorCoordinates[index].x - 1; i < vectorCoordinates[index].x + 2; i++)
					for (int j = vectorCoordinates[index].y - 1; j < vectorCoordinates[index].y + 2; j++)
						for (int k = vectorCoordinates[index].z - 1; k < vectorCoordinates[index].z + 2; k++)
							if (tab[i][j][k] != 0 && tab[i][j][k] != tab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z])
								counter++;
				Eprev = counter + energyTab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z];
				counter = 0;
				for (int i = vectorCoordinates[index].x - 1; i < vectorCoordinates[index].x + 2; i++)
					for (int j = vectorCoordinates[index].y - 1; j < vectorCoordinates[index].y + 2; j++)
						for (int k = vectorCoordinates[index].z - 1; k < vectorCoordinates[index].z + 2; k++)
							if (tab[i][j][k] != 0 && tab[i][j][k] != idTmp)
								counter++;
				Enow = counter;

				if (Enow - Eprev > 0) {
					tab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z] = idTmp;
					energyTab[vectorCoordinates[index].x][vectorCoordinates[index].y][vectorCoordinates[index].z] = 0;
				}
			}
			vectorCoordinates.erase(vectorCoordinates.begin() + index);
			iterationsCounter++;
		}
		counter = 0;
	}
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	drawCubes();
}

void ASimulationController::setGrains(int grainsTmp)
{
	this->grains = grainsTmp;
}

void ASimulationController::setSize(int sizeTmp)
{
	this->size = sizeTmp;
}

void ASimulationController::setScale(float scaleTmp)
{
	this->scale = scaleTmp;
}

void ASimulationController::initiate(int grainsTmp, int sizeTmp, float scaleTmp)
{
	height = sizeTmp + 1;
	mostTab = new int[grainsTmp + 2];
	clearMostTab();
	setScale(scaleTmp);
	setGrains(grainsTmp);
	setSize(sizeTmp+2);
	colorTable = ColorTab(grains);

	tab = new int**[size];
	for (int i = 0; i < size; i++)
		tab[i] = new int*[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			tab[i][j] = new int[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				tab[i][j][k] = 0;

	tabOld = new int**[size];
	for (int i = 0; i < size; i++)
		tabOld[i] = new int*[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			tabOld[i][j] = new int[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				tabOld[i][j][k] = 0;

	energyTab = new double**[size];
	for (int i = 0; i < size; i++)
		energyTab[i] = new double*[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			energyTab[i][j] = new double[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				energyTab[i][j][k] = 0;
}

void ASimulationController::setup()
{
	int spawnedGrains = 0;
	while(spawnedGrains != grains){
		int x = (rand() % (size - 2)) + 1;
		int y = (rand() % (size - 2)) + 1;
		int z = (rand() % (size - 2)) + 1;
		if (tab[x][y][z] == 0) {
			tab[x][y][z] = 2+spawnedGrains;
			spawnCube(x, y, z, 2+spawnedGrains);
			spawnedGrains++;
		}
	}
}

void ASimulationController::computeStep()
{
	rewriteTab();
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++) {
				if (tabOld[i][j][k] == 0) {
					newId = checkMost(i, j, k);
					if (newId > 1) {
						tab[i][j][k] = newId;
						spawnCube(i, j, k, newId);
					}
				}
			}
}

void ASimulationController::computeNonOptimal()
{
	FDateTime time1 = returnTime();
	while (!checkIfDone()) {
		computeStep();
	}
	FDateTime time2 = returnTime();
	FLinearColor color(0, 0, 0);
	UKismetSystemLibrary::PrintString(GetWorld(), (FString)("Duration time") + FString::FromInt(ComputeTime(time2,time1)), true, false, color, 10);
}

bool ASimulationController::checkIfDone()
{
	for(int i = 1; i < size - 1; i++)
		for(int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				if(tab[i][j][k] == 0)
					return false;
	return true;
}

void ASimulationController::computeOptimal(int inclusions, int radius, bool circle, bool boundaries, int chance)
{
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	FLinearColor color(0, 0, 0);
	FDateTime time1 = returnTime();
	std::vector<Coordinates> vectorCoordinates;
	int counter = 0;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++) {
				vectorCoordinates.insert(vectorCoordinates.begin() + counter, Coordinates(i, j, k));
				counter++;
			}

	int i = 0, j = 0, k = 0;
	for (int l = vectorCoordinates.size() - 1; l > - 1; l--) {
		i = vectorCoordinates[l].x;
		j = vectorCoordinates[l].y;
		k = vectorCoordinates[l].z;
		if (tab[i][j][k] != 0)
			vectorCoordinates.erase(vectorCoordinates.begin() + l);
	}
	while(vectorCoordinates.size()) {
			rewriteTab();
			for (int l = vectorCoordinates.size() - 1; l > -1; l--) {
				i = vectorCoordinates[l].x;
				j = vectorCoordinates[l].y;
				k = vectorCoordinates[l].z;
				newId = checkMost(i, j, k);
				if (rand() % 101 <= chance)
					if (newId > 1) {
						tab[i][j][k] = newId;
						vectorCoordinates.erase(vectorCoordinates.begin() + l);
					}
			}
		}
	if(boundaries)
		grainsBoundaries();
	afterSimulationInclusion(inclusions, radius, circle);
	drawCubes();
	FDateTime time2 = returnTime();
	UKismetSystemLibrary::PrintString(GetWorld(), (FString)("Duration time") + FString::FromInt(ComputeTime(time2, time1)), true, false, color, 10);
}

void ASimulationController::drawCubes()
{
	//BOTTOM
	for (int i = 2; i < size - 2; i++)
		for (int j = 2; j < size - 2; j++)
			spawnCube(i, j, 1, tab[i][j][1]);
	//TOP
	for (int i = 2; i < size - 2; i++)
		for (int j = 2; j < size - 2; j++)
			spawnCube(i, j, size - 2, tab[i][j][size-2]);
	//LEFT
	for (int i = 2; i < size - 2; i++)
		for (int j = 2; j < size - 2; j++)
			spawnCube(i, 1, j, tab[i][1][j]);
	//RIGHT
	for (int i = 2; i < size - 2; i++)
		for (int j = 2; j < size - 2; j++)
			spawnCube(i, size - 2, j, tab[i][size - 2][j]);
	//FRONT
	for (int i = 2; i < size - 2; i++)
		for (int j = 2; j < size - 2; j++)
			spawnCube(1, i, j, tab[1][i][j]);
	//BACK
	for (int i = 2; i < size - 2; i++)
		for (int j = 2; j < size - 2; j++)
			spawnCube(size - 2, i, j, tab[size - 2][i][j]);
	//EDGES
	for (int i = 2; i < size - 2; i++) {
		spawnCube(i, 1, 1, tab[i][1][1]);
		spawnCube(i, 1, size - 2, tab[i][1][size - 2]);
		spawnCube(i, size - 2, 1, tab[i][size - 2][1]);
		spawnCube(i, size - 2, size - 2, tab[i][size - 2][size - 2]);

		spawnCube(1, i, 1, tab[1][i][1]);
		spawnCube(1, i, size - 2, tab[1][i][size - 2]);
		spawnCube(size - 2, i, 1, tab[size - 2][i][1]);
		spawnCube(size - 2, i, size - 2, tab[size - 2][i][size - 2]);

		spawnCube(1, 1, i, tab[1][1][i]);
		spawnCube(1, size - 2, i, tab[1][size - 2][i]);
		spawnCube(size - 2, 1, i, tab[size - 2][1][i]);
		spawnCube(size - 2, size - 2, i, tab[size - 2][size - 2][i]);
	}
	//CORNERS
	spawnCube(1, 1, 1, tab[1][1][1]);
	spawnCube(1, size - 2, 1, tab[1][size - 2][1]);
	spawnCube(1, 1, size - 2, tab[1][1][size - 2]);
	spawnCube(size - 2, 1, 1, tab[size - 2][1][1]);
	spawnCube(size - 2, 1, size - 2, tab[size - 2][1][size - 2]);
	spawnCube(1, size - 2, size - 2, tab[1][size - 2][size - 2]);
	spawnCube(size - 2, size - 2, 1, tab[size - 2][size - 2][1]);
	spawnCube(size - 2, size - 2, size - 2, tab[size - 2][size - 2][size - 2]);
}

void ASimulationController::reComputeOptimal(int grainsTmp, int inclusions, int radius, bool circle, bool boundaries, int chance)
{
	CubeColor* tmp = new CubeColor[grainsTmp + 3];
	for (int i = 0; i < grainsTmp + 3; i++)
		tmp[i] = colorTable.tab[i];

	setGrains(grainsTmp);
	delete[] mostTab;
	mostTab = new int[grains + 3];
	bool condition = false;
	bool isSpawned = false;
	for (int i = 2; i < grains + 2; i++) {
		for (int j = 0; j < cubeVector[0]->idVector.size(); j++)
			if (i == cubeVector[0]->idVector[j])
				condition = true;
		if (!condition) {
			tmp[i] = CubeColor((float)(rand() % 101) / 100., (float)(rand() % 101) / 100., (float)(rand() % 101) / 100.);
		}
		condition = false;
	}
	delete[] colorTable.tab;
	colorTable.tab = tmp;
	condition = false;
	for (int i = 2; i < grains + 2; i++) {
		for (int j = 0; j < cubeVector[0]->idVector.size(); j++)
			if (i == cubeVector[0]->idVector[j])
				condition = true;
		if(!condition)
			while (!isSpawned) {
				int x = (rand() % (size - 2)) + 1;
				int y = (rand() % (size - 2)) + 1;
				int z = (rand() % (size - 2)) + 1;
				if (tab[x][y][z] == 0) {
					tab[x][y][z] = i;
					spawnCube(x, y, z, i);
					isSpawned = true;
				}
			}
		isSpawned = false;
		condition = false;
	}
	computeOptimal(inclusions, radius, circle, boundaries, chance);
}

void ASimulationController::dualPhase(int grainsTmp, int inclusions, int radius, bool circle, bool boundaries, int chance)
{
	CubeColor* tmp = new CubeColor[grainsTmp + 3];
	tmp[2] = colorTable.tab[cubeVector[0]->idVector[0]];
	tmp[0] = CubeColor(1, 1, 1);
	tmp[1] = CubeColor(0, 0, 0);
	tmp[grains+2] = CubeColor(1, 0, 0);

	setGrains(grainsTmp);
	delete[] mostTab;
	mostTab = new int[grains + 3];
	bool isSpawned = false;
	for (int i = 3; i < grains + 2; i++)
		tmp[i] = CubeColor((float)(rand() % 101) / 100., (float)(rand() % 101) / 100., (float)(rand() % 101) / 100.);
	delete[] colorTable.tab;
	colorTable.tab = tmp;

	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				if (tab[i][j][k] > 2)
					tab[i][j][k] = 2;

	for (int i = 3; i < grains + 2; i++) {
			while (!isSpawned) {
				int x = (rand() % (size - 2)) + 1;
				int y = (rand() % (size - 2)) + 1;
				int z = (rand() % (size - 2)) + 1;
				if (tab[x][y][z] == 0) {
					tab[x][y][z] = i;
					spawnCube(x, y, z, i);
					isSpawned = true;
				}
			}
		isSpawned = false;
	}
	//computeOptimal(inclusions, radius, circle, boundaries, chance);
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	FLinearColor color(0, 0, 0);
	FDateTime time1 = returnTime();
	std::vector<Coordinates> vectorCoordinates;
	int counter = 0;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++) {
				vectorCoordinates.insert(vectorCoordinates.begin() + counter, Coordinates(i, j, k));
				counter++;
			}

	int i = 0, j = 0, k = 0;
	for (int l = vectorCoordinates.size() - 1; l > -1; l--) {
		i = vectorCoordinates[l].x;
		j = vectorCoordinates[l].y;
		k = vectorCoordinates[l].z;
		if (tab[i][j][k] != 0)
			vectorCoordinates.erase(vectorCoordinates.begin() + l);
	}
	while (vectorCoordinates.size()) {
		rewriteTab();
		for (int l = vectorCoordinates.size() - 1; l > -1; l--) {
			i = vectorCoordinates[l].x;
			j = vectorCoordinates[l].y;
			k = vectorCoordinates[l].z;
			newId = checkMostMonteCarlo(i, j, k);
			if (rand() % 101 <= chance)
				if (newId > 1) {
					tab[i][j][k] = newId;
					vectorCoordinates.erase(vectorCoordinates.begin() + l);
				}
		}
	}
	if (boundaries)
		grainsBoundaries();
	drawCubes();
}

void ASimulationController::beforeSimulationInclusion(int inclusions, int radius, bool circle)
{
	rewriteTab();
	int inclusionsTmp = inclusions;
	while (inclusionsTmp > 0) {
		int x = (rand() % (size - 2)) + 1;
		int y = (rand() % (size - 2)) + 1;
		int z = (rand() % (size - 2)) + 1;
		if (tab[x][y][z] == 0) {
			if (circle)
				circleInclusion(x,y,z,radius);
			else
				squareInclusin(x, y, z, radius);
			inclusionsTmp--;
		}
	}
}

void ASimulationController::afterSimulationInclusion(int inclusions, int radius, bool circle)
{
	rewriteTab();
	int inclusionsTmp = inclusions;
	while (inclusionsTmp > 0) {
		int x = (rand() % (size - 2)) + 1;
		int y = (rand() % (size - 2)) + 1;
		int z = (rand() % (size - 2)) + 1;

		if (tabOld[x - 1][y][z] != tabOld[x][y][z] && tabOld[x - 1][y][z] != 0)
			if (tab[x][y][z] == 1) {
				if (circle)
					circleInclusion(x, y, z, radius);
				else
					squareInclusin(x, y, z, radius);
				inclusionsTmp--;
			}
		else if (tabOld[x + 1][y][z] != tabOld[x][y][z] && tabOld[x + 1][y][z] != 0)
				if (tab[x][y][z] == 1) {
					if (circle)
						circleInclusion(x, y, z, radius);
					else
						squareInclusin(x, y, z, radius);
					inclusionsTmp--;
				}
		else if (tabOld[x][y - 1][z] != tabOld[x][y][z] && tabOld[x][y - 1][z] != 0)
					if (tab[x][y][z] == 1) {
						if (circle)
							circleInclusion(x, y, z, radius);
						else
							squareInclusin(x, y, z, radius);
						inclusionsTmp--;
					}
		else if (tabOld[x][y + 1][z] != tabOld[x][y][z] && tabOld[x][y + 1][z] != 0)
						if (tab[x][y][z] == 1) {
							if (circle)
								circleInclusion(x, y, z, radius);
							else
								squareInclusin(x, y, z, radius);
							inclusionsTmp--;
						}
		else if (tabOld[x][y][z - 1] != tabOld[x][y][z] && tabOld[x][y][z - 1] != 0)
							if (tab[x][y][z] == 1) {
								if (circle)
									circleInclusion(x, y, z, radius);
								else
									squareInclusin(x, y, z, radius);
								inclusionsTmp--;
							}
		else if (tabOld[x][y][z + 1] != tabOld[x][y][z] && tabOld[x][y][z + 1] != 0)
								if (tab[x][y][z] == 1) {
									if (circle)
										circleInclusion(x, y, z, radius);
									else
										squareInclusin(x, y, z, radius);
									inclusionsTmp--;
								}
	}
}

void ASimulationController::squareInclusin(int x, int y, int z, int radius)
{
	for (int i = x - (radius / 2); i < x + (radius / 2) + 1; i++)
		for (int j = y - (radius / 2); j < y + (radius / 2) + 1; j++)
			for (int k = z - (radius / 2); k < z + (radius / 2) + 1; k++) {
				if (i<1 || j<1 || k<1 || i>size - 2 || j>size - 2 || k>size - 2)
					continue;
				tab[i][j][k] = 1;
				spawnCube(i, j, k, 1);
			}
}

void ASimulationController::circleInclusion(int x, int y, int z, int radius)
{
	for (int i = x - radius; i < x + radius + 1; i++)
		for (int j = y - radius; j < y + radius + 1; j++)
			for (int k = z - radius; k < z + radius + 1; k++){
				if (i<1 || j<1 || k<1 || i>size - 2 || j>size - 2 || k>size - 2)
					continue;
				if ((i - x)*(i - x) + (j - y)*(j - y) + (k - z)*(k - z) <= radius * radius) {
					tab[i][j][k] = 1;
					spawnCube(i, j, k, 1);
				}
			}
}


void ASimulationController::grainsBoundaries()
{
	rewriteTab();
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++) {
				if (tabOld[i - 1][j][k] != tabOld[i][j][k] && tabOld[i - 1][j][k] != 0)
					tab[i][j][k] = 1;
				else if (tabOld[i + 1][j][k] != tabOld[i][j][k] && tabOld[i + 1][j][k] != 0)
					tab[i][j][k] = 1;
				else if (tabOld[i][j - 1][k] != tabOld[i][j][k] && tabOld[i][j - 1][k] != 0)
					tab[i][j][k] = 1;
				else if (tabOld[i][j + 1][k] != tabOld[i][j][k] && tabOld[i][j + 1][k] != 0)
					tab[i][j][k] = 1;
				else if (tabOld[i][j][k - 1] != tabOld[i][j][k] && tabOld[i][j][k - 1] != 0)
					tab[i][j][k] = 1;
				else if (tabOld[i][j][k + 1] != tabOld[i][j][k] && tabOld[i][j][k + 1] != 0)
					tab[i][j][k] = 1;
			}
}

void ASimulationController::printPercentageOfBoundaries()
{
	FLinearColor color(0, 0, 0);
	int counter = 0;
	for (int i = 1; i < size - 1; i++)
		for (int j = 1; j < size - 1; j++)
			for (int k = 1; k < size - 1; k++)
				if (tab[i][j][k] == 1)
					counter++;
	UKismetSystemLibrary::PrintString(GetWorld(),(FString)("Percent of boundaries: ") + FString::SanitizeFloat((((float)counter/(float)((size-1)*(size - 1)*(size - 1)))*100)), true, false, color, 2);
}

void ASimulationController::reset()
{
	resetIdVector();
	for (int i = 0; i < cubeVector.size(); i++) {
		cubeVector[i]->Destroy();
	}
	cubeVector.clear();
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++) {
			delete[] tab[i][j];
			delete[] tabOld[i][j];
			delete[] energyTab[i][j];
		}
	for (int i = 0; i < size; i++) {
		delete[] tab[i];
		delete[] tabOld[i];
		delete[] energyTab[i];
	}
	delete[] tab;
	delete[] tabOld;
	delete[] energyTab;
	delete[] mostTab;
	delete[] colorTable.tab;
}

void ASimulationController::deleteSome()
{
	bool condition = false;
	if (cubeVector.size())
		for (int i = 1; i < size - 1; i++)
			for (int j = 1; j < size - 1; j++)
				for (int k = 1; k < size - 1; k++) {
					for (int l = 0; l < cubeVector[0]->idVector.size(); l++)
						if (tab[i][j][k] == cubeVector[0]->idVector[l])
							condition = true;
					if (!condition)
						tab[i][j][k] = 0;
					condition = false;
				}
	if (cubeVector.size())
		for (int i = cubeVector.size() - 1; i > -1; i--) {
			for (int j = 0; j < cubeVector[0]->idVector.size(); j++)
				if (cubeVector[i]->id == cubeVector[0]->idVector[j])
					condition = true;
			if (!condition) {
				cubeVector[i]->Destroy();
				cubeVector.erase(cubeVector.begin() + i);
			}
			condition = false;
		}
}

void ASimulationController::resetIdVector()
{
	if (cubeVector.size())
		cubeVector[0]->idVector.clear();
}

FDateTime ASimulationController::returnTime()
{
	return FDateTime::Now();
}

int ASimulationController::ComputeTime(FDateTime time2, FDateTime time1)
{
	int timeTmp2 = time2.GetMillisecond() + time2.GetSecond() * 1000 + time2.GetMinute() * 60000 + time2.GetHour() * 3600000;
	int timeTmp1 = time1.GetMillisecond() + time1.GetSecond() * 1000 + time1.GetMinute() * 60000 + time1.GetHour() * 3600000;
	return timeTmp2 - timeTmp1;
}

void ASimulationController::rewriteTab()
{
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				tabOld[i][j][k] = tab[i][j][k];
}

int ASimulationController::checkMost(int x, int y, int z)
{
	clearMostTab();
	mostTab[tabOld[x-1][y][z]]++;
	mostTab[tabOld[x+1][y][z]]++;
	mostTab[tabOld[x][y-1][z]]++;
	mostTab[tabOld[x][y+1][z]]++;
	mostTab[tabOld[x][y][z-1]]++;
	mostTab[tabOld[x][y][z+1]]++;


	for (int i = 2; i < grains + 2; i++)
		if (mostTab[i] > max) {
			max = mostTab[i];
			maxId = i;
		}
	return maxId;
}

int ASimulationController::checkMostMonteCarlo(int x, int y, int z)
{
	clearMostTab();
	mostTab[tabOld[x - 1][y][z]]++;
	mostTab[tabOld[x + 1][y][z]]++;
	mostTab[tabOld[x][y - 1][z]]++;
	mostTab[tabOld[x][y + 1][z]]++;
	mostTab[tabOld[x][y][z - 1]]++;
	mostTab[tabOld[x][y][z + 1]]++;

	mostTab[2] = 0;

	for (int i = 2; i < grains + 2; i++)
		if (mostTab[i] > max) {
			max = mostTab[i];
			maxId = i;
		}
	return maxId;
}

int ASimulationController::checkMostRecrystalisation(int x, int y, int z)
{
	int maxTmp = 0;
	int* recrystalisationTab = new int[10];
	for (int i = 0; i < 10; i++)
		recrystalisationTab[i] = 0;

	if (tab[x - 1][y][z] > grains + 2)
		maxId = tab[x - 1][y][z];
	else if (tab[x + 1][y][z] > grains + 2)
		maxId = tab[x + 1][y][z];
	else if (tab[x][y - 1][z] > grains + 2)
		maxId = tab[x][y - 1][z];
	else if (tab[x][y + 1][z] > grains + 2)
		maxId = tab[x][y + 1][z];
	else if (tab[x][y][z - 1] > grains + 2)
		maxId = tab[x][y][z - 1];
	else if (tab[x][y][z + 1] > grains + 2)
		maxId = tab[x][y][z + 1];
	else {
		delete[] recrystalisationTab;
		return 0;
	}
	recrystalisationTab[tab[x - 1][y][z] - grains - 2]++;
	recrystalisationTab[tab[x + 1][y][z] - grains - 2]++;
	recrystalisationTab[tab[x][y - 1][z] - grains - 2]++;
	recrystalisationTab[tab[x][y + 1][z] - grains - 2]++;
	recrystalisationTab[tab[x][y][z - 1] - grains - 2]++;
	recrystalisationTab[tab[x][y][z + 1] - grains - 2]++;

	for (int i = 0; i < 10; i++)
		if (recrystalisationTab[i] > maxTmp) {
			maxTmp = recrystalisationTab[i];
			maxId = i + grains + 2;
		}
	delete[] recrystalisationTab;
	return maxId;
}

void ASimulationController::clearMostTab()
{
	for (int i = 0; i < grains + 2; i++)
		mostTab[i] = 0;
	max = 0;
	maxId = 0;
}

bool ASimulationController::Init()
{
	return true;
}

uint32 ASimulationController::Run()
{
	FLinearColor color(0, 0, 0);
	for (int i = 0; i < 10; i++) {
		UKismetSystemLibrary::PrintString(GetWorld(), FString("Wiadomosc z watku"), true, false, color, 20);
		FPlatformProcess::Sleep(0.3);
	}
	return 0;
}

void ASimulationController::callThread()
{
	thread1 = FRunnableThread::Create(this, TEXT("Test thread"), 0, TPri_Normal);
	thread1->WaitForCompletion();
	FLinearColor color(0, 0, 0);
	UKismetSystemLibrary::PrintString(GetWorld(), FString("xD"), true, false, color, 20);
}

void ASimulationController::Stop()
{
}

Coordinates::Coordinates()
{
	x = 0;
	y = 0;
	z = 0;
}

Coordinates::Coordinates(int x, int y , int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
