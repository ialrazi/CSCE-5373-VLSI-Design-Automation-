#include <iostream>
#include <vector>
#include <cstdlib>
#include <utility>
#include <fstream>
#include "DataStruct.h"
#include "Defs.h"
#include "helperFuncs.h"
using namespace std;



unordered_map<int,snBlock*> blocks;
char wait;

// should be taken from GUI
char spgFile[MAX_FILEN];
double initTemp;
double alpha;
double beta;
time_t maxTime;
bool snapLabel;

void readConfig(char *filename)
{
	ifstream file(filename);
	if(!file) {
		cout << "Failed to open file: " << filename << endl;
		exit(1);
	}
	file >> spgFile;
	file >> initTemp;
	file >> alpha;
	file >> beta;
	file >> maxTime;
	file >> snapLabel;
}



int main(int argc, char *argv[])
{
	if(argc < 2) {
		cout << "Usage: furnace <config-file-path>" << endl;
		exit(0);
	}

	// read configuration
	cout << "Loading configuration..." << endl;
	readConfig(argv[1]);
	cout << "SPG File   : " << spgFile << endl;
	cout << "Temperature: " << initTemp << endl;
	cout << "Alpha      : " << alpha << endl;
	cout << "Beta       : " << beta << endl;
	cout << "Time limit : " << maxTime << " seconds\n\n";
	
	// read block information
	cout << "Loading initial Floorplan..." << endl;
	snSP sp;
	readSPG(spgFile, blocks, sp);
	sp.blocks = &blocks;
	sp.rebuild();
	sp.updateLocations();
	sp.snapShotBoundary = max(sp.width, sp.height);
	//printBlocks(&blocks);
	//sp.printInfo(); 
	cout << "No. of Blocks: " << sp.noOfBlocks << endl;
	cout << "Area         : " << sp.area << endl;
	cout << "Dimensions   : (" << sp.width << ", " << sp.height << ")\n";
	cout << "Aspect ratio : " << sp.width/sp.height << endl;
	cout << "Utilization  : " << sp.cellArea/sp.area * 100 << "%\n\n";
	sp.storeSnapShot("../outputs/initFP.gnu", snapLabel);
	
	// prepare the furnace
	snFurnace furnace;
	furnace.SP = &sp;
	furnace.loadDefaults();		// edit on top of defaults
	furnace.T = initTemp;
	furnace.alpha = alpha;
	furnace.beta = beta;
	furnace.snapLabel = snapLabel;
	furnace.maxTime = maxTime;

	// perform annealing
	cout << "\n\nStarting Annealing..." << endl;
	furnace.anneal();
	
	// restore the best solution
	furnace.bestSP.rebuild();
	furnace.bestSP.snapShotBoundary = sp.snapShotBoundary;
	furnace.bestSP.storeSnapShot("../outputs/best.gnu",snapLabel);
	furnace.genPlt();
	saveFP(&blocks, "../outputs/Floorplan.fp");


	return 0;
}


