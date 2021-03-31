#include <iostream>
#include <fstream>
#include "Defs.h"
#include "DataStruct.h"
#include "helperFuncs.h"
using namespace std;


// Reads the SPG format file and stores the data in a blocks pointer vector
// stores the sequence pair in the sp reference
bool readSPG(const char *filename, std::unordered_map<int,snBlock*> &blocks, snSP &sp)
{
	ifstream file(filename);
	if(!file) {
		cout << "Failed to open file: " << filename << endl;
		exit(1);
		return false;
	}

	int moduleNo;
	file >> moduleNo;	// read the number of modules
	sp.noOfBlocks = moduleNo;
	sp.cellArea = 0;

	// read the +ve sequence
	int temp;
	for(int i=0; i<moduleNo; i++) {
		file >> temp;
		sp.posSeq.push_back(temp);
	}

	// read negative sequence
	for(int i=0; i<moduleNo; i++) {
		file >> temp;
		sp.negSeq.push_back(temp);
	}

	// read module info
	snBlock* blkBuff;
	int nodeCnt;
	for(int i=0; i<moduleNo; i++) {
		blkBuff = new snBlock;
		file >> blkBuff->id;
		file >> blkBuff->width;
		file >> blkBuff->height;
		sp.cellArea += blkBuff->width * blkBuff->height;
		// HCG parents
		file >> nodeCnt;	// number of parent nodes
		for(int j=0; j<nodeCnt; j++) {
			file >> temp;
			blkBuff->HCGParents[temp] = nullptr;	// initially assign a null pointer
		}
		// HCG Children
		file >> nodeCnt;	// number of child nodes
		for(int j=0; j<nodeCnt; j++) {
			file >> temp;
			blkBuff->HCGChildren[temp] = nullptr;	// initially assign a null pointer
		}
		// VCG parents
		file >> nodeCnt;	// number of parent nodes
		for(int j=0; j<nodeCnt; j++) {
			file >> temp;
			blkBuff->VCGParents[temp] = nullptr;	// initially assign a null pointer
		}
		// VCG Children
		file >> nodeCnt;	// number of child nodes
		for(int j=0; j<nodeCnt; j++) {
			file >> temp;
			blkBuff->VCGChildren[temp] = nullptr;	// initially assign a null pointer
		}

		blocks[blkBuff->id] = blkBuff;				// store the pointer with the id	
	}

	// read source children
	snBlock* source = new snBlock;
	file >> nodeCnt;		// in HCG
	for(int i=0; i<nodeCnt; i++) {
		file >> temp;
		source->HCGChildren[temp] = nullptr;			// initially assign a null pointer
	}
	file >> nodeCnt;		// in VCG
	for(int i=0; i<nodeCnt; i++) {
		file >> temp;
		source->VCGChildren[temp] = nullptr;			// initially assign a null pointer
	}
	source->id = SOURCE_ID;
	blocks[SOURCE_ID] = source;

	// read target parents
	snBlock* target = new snBlock;
	file >> nodeCnt;		// in HCG
	for(int i=0; i<nodeCnt; i++) {
		file >> temp;
		target->HCGParents[temp] = nullptr;			// initially assign a null pointer
	}
	file >> nodeCnt;		// in VCG
	for(int i=0; i<nodeCnt; i++) {
		file >> temp;
		target->VCGParents[temp] = nullptr;			// initially assign a null pointer
	}
	target->id = TARGET_ID;
	blocks[TARGET_ID] = target;

	file.close();	// close the file
	
	// update the SP indices
	for(int i=0; i<sp.noOfBlocks; i++){
		blocks[sp.posSeq[i]]->posSeqIndx = i;
		blocks[sp.negSeq[i]]->negSeqIndx = i;
	}
	
	// update the CG pointers
	for(auto &blk : blocks){
		for(auto &p : blk.second->HCGParents){
			p.second = blocks[p.first];
		}
		for(auto &p : blk.second->HCGChildren){
			p.second = blocks[p.first];
		}
		for(auto &p : blk.second->VCGParents){
			p.second = blocks[p.first];
		}
		for(auto &p : blk.second->VCGChildren){
			p.second = blocks[p.first];
		}
	}
	
	return true;
}




// prints the sequence pair and all the block info
void printBlocks(std::unordered_map<int,snBlock*> *blocks)
{
	cout << "printBlocks check 1" << endl;
	// print block info
	for(auto &blk : *blocks) {
		blk.second->printInfo();
		cout << endl;
	}	
	cout << endl;
}






// generates a gnuplot script to show the floorplan
void genGnuPlot(unordered_map<int,snBlock*> *blocks, int width, int height, const char *filename, bool label)
{
	ofstream file(filename);
	if(!file) {
		cout << "Failed to open file: " << filename << endl;
		exit(1);
	}
	
	char color1[20];
	char color2[20];
	snBlock *blk;
	for(auto &blkp : *blocks) {
		if(blkp.first<0) continue;			// skip the source and terminal
		randColor(color1, color2);
		blk = blkp.second;
		// draw the rectangle
		file << "set obj " << blk->id+1;
		file << " rect from " << blk->locX << "," << blk->locY;
		file << " to " << (blk->locX+blk->width) << "," << blk->locY+blk->height;
		file << " fs solid fc rgb \"#" << color1  << "\"" << endl;

		if(label) {
			// put labels
			file << "set label " << blk->id+1;
			file << " at " << blk->locX+blk->width/2 << "," << blk->locY+blk->height/2;
			file << " \"m" << blk->id << "\" center tc rgb \"#" << color2 << "\"" << endl; 
		}
	}
	int tics = height/GRAPH_TIC;
	file << "set ytics " << tics << endl;
	file << "set grid ytics lc rgb \"#bbbbbb\" lw 1 lt 0" << endl;
	file << "set xtics " << tics << " rotate" << endl;
	file << "set grid xtics lc rgb \"#bbbbbb\" lw 1 lt 0" << endl;
	file << "set terminal png size 400,400" << endl;
	file << "set output \'" << filename << ".png\'" << endl;
	file << "plot [0:" << width << "][0:" << height << "] 0 notitle" << endl;
	file << "show grid" << endl;
}





void randColor(char *color1, char *color2)
{
	int num = rand() * rand();
	//num &= 0x7F7F7F;	// turn off all upper bits, dark shade
	sprintf(color1, "%x", num);
	num |= 0x808080;	// create a light shade of the color1
	sprintf(color2, "%x", num);
}



void saveFP(unordered_map<int,snBlock*> *blocks, const char *filename)
{
	ofstream file(filename);
	if(!file) {
		cout << "Failed to open file: " << filename << endl;
		exit(1);
	}

	file << "********************************************************\n";
	file << "*HARDBLOCKS\n";

	snBlock *blk;
	for(auto &blkp : *blocks) {
		if(blkp.first<0) continue;			// skip the source and terminal
		blk = blkp.second;
		file << blk->id << " \t" << blk->locX << " \t" << blk->locY << endl;
	} 

	file << "*END\n";
	file << "********************************************************\n";
	
}
