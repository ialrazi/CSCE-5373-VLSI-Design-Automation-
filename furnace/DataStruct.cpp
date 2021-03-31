#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "Defs.h"
#include "DataStruct.h"
#include "helperFuncs.h"
using namespace std;



//------------ snBlock Member functions ----------------
void snBlock::printInfo()
{
	cout << "Block: " << this->id << endl;
	cout << "Dim: (" << this->width << ", " << this->height << ")" << endl;
	cout << "Rotated: " << this->rotation << endl;
	cout << "Loc: (" << this->locX << ", " << this->locY << ")" << endl;
	cout << "Ind: (" << this->posSeqIndx << ", " << this->negSeqIndx << ")" <<endl;
	// Print the HCG info
	cout << "HCG: ";
	for(auto &i : this->HCGParents) cout << i.first << " ";
	cout << ": ";
	for(auto &i : this->HCGChildren) cout << i.first << " ";
	cout << endl;
	// Print the VCG info
	cout << "VCG: ";
	for(auto &i : this->VCGParents) cout << i.first << " ";
	cout << ": ";
	for(auto &i : this->VCGChildren) cout << i.first << " ";
	cout << endl; 
}

void snBlock::rotate()
{
	this->rotation = !this->rotation;
	swap(width, height);
}




//------------ snSP Member functions ----------------
void snSP::printInfo()
{
	cout << "Blocks : " << noOfBlocks << endl;
	cout << "Pos Seq: ";
	for(auto id: posSeq) cout << id << " ";
	cout << endl;
	cout << "Neg Seq: ";
	for(auto id: negSeq) cout << id << " ";
	cout << endl;
	cout << "Area   : " << area << endl;
	cout << "Dim    : (" << width << ", " << height << ")" << endl;
	cout << "Aspect : " << width*1.0/height << endl;
	cout << "Density: " << cellArea/area * 100 << "%" << endl;
}


void snSP::clear()
{
	this->noOfBlocks = 0;
	this->posSeq.clear();
	this->negSeq.clear();
	this->width = 0;
	this->height = 0;
	this->cellArea = 0;
	this->area = 0;
	this->blocks = nullptr;
}


void snSP::clone(snSP &ob)
{
	this->noOfBlocks = ob.noOfBlocks;
	this->posSeq = ob.posSeq;
	this->negSeq = ob.negSeq;
	this->width  = ob.width;
	this->height = ob.height;
	this->area   = ob.area;
	this->cellArea = ob.cellArea;
	this->blocks = ob.blocks;
}





//------------ snMove Member functions ----------------
void snMove::printInfo()
{
	cout << moveNames[this->type] << " ";
	cout << this->blk1_id << " " << this->blk2_id;
	cout << endl;
}


// inverts the move
void snMove::invert()
{
	if(type!=rotate)
		swap(blk1_id, blk2_id);
}




//------------ snSP Member functions ----------------
void snSP::applyMove(snMove &move)
{
	int temp;
	int indx1, indx2;
	bool update = false;		// need to update the graph
	snBlock *blk1 = (*blocks)[move.blk1_id];
	snBlock *blk2 = (*blocks)[move.blk2_id];
	
	//cout << "applyMove  Check: 1" << endl;

	switch(move.type) {
	case swap_ps:
		indx1 = blk1->posSeqIndx;
		indx2 = blk2->posSeqIndx;
		swap(posSeq[indx1], posSeq[indx2]);
		swap(blk1->posSeqIndx, blk2->posSeqIndx);		
		update = true;
		break;

	case swap_ns:
		indx1 = blk1->negSeqIndx;
		indx2 = blk2->negSeqIndx;
		swap(negSeq[indx1], negSeq[indx2]);
		swap(blk1->negSeqIndx, blk2->negSeqIndx);		
		update = true;
		break;

	case rotate:
		blk1->rotation = !blk1->rotation;
		swap(blk1->width, blk1->height);
		break;
	}
	if(update) {
		int srtIndx = min(indx1, indx2);
		int stpIndx = max(indx1, indx2);
		int nodCnt = stpIndx - srtIndx + 1; // inclusive interval
		// get the intermediate blocks
		vector<int> intermid(nodCnt);
		if(move.type == swap_ps) {
			for(int i=0, indx=srtIndx; i<nodCnt; i++) {
				intermid[i] = posSeq[indx++];
			} 
		}else {
			for(int i=0, indx=srtIndx; i<nodCnt; i++) {
				intermid[i] = negSeq[indx++];
			} 
		}
		//for(auto id : intermid) cout << id << " ";
		//cout << endl;
		// update relative subgraphs relative to blk1 & blk2
		//cout << "applyMove  Check: 2" << endl;
		updateSubGraph(blk1, intermid);
		//cout << "applyMove  Check: 3" << endl;
		updateSubGraph(blk2, intermid);
		//cout << "applyMove  Check: 4" << endl;

	}

}



void snSP::updateSubGraph(snBlock *blk, vector<int> &nodesID)
{
	snBlock *otherBlk;
	int id_cur = blk->id;
	int id_oth;
	for(auto id : nodesID) {
		otherBlk = (*blocks)[id];
		id_oth = id;
		if( id_cur == id_oth ) continue;	// skip ownself

		//cout << "updateSubGraph Check: 1" << endl;
		// remove old connections
		otherBlk->HCGParents.erase(blk->id);
		otherBlk->HCGChildren.erase(blk->id);
		otherBlk->VCGParents.erase(blk->id);
		otherBlk->VCGChildren.erase(blk->id);
		blk->HCGParents.erase(otherBlk->id);
		blk->HCGChildren.erase(otherBlk->id);
		blk->VCGParents.erase(otherBlk->id);
		blk->VCGChildren.erase(otherBlk->id);
		

		//cout << "updateSubGraph Check: 2" << endl;
		// create an edge based on relative location
		int psRelLoc = blk->posSeqIndx - otherBlk->posSeqIndx;
		int nsRelLoc = blk->negSeqIndx - otherBlk->negSeqIndx;
		if( psRelLoc<0 && nsRelLoc<0 ) {
			// blk is on left of otherBlk
			blk->HCGChildren[id_oth] = otherBlk;
			otherBlk->HCGParents[id_cur] = blk; 
		} else if ( psRelLoc<0 && nsRelLoc>0 ) {
			// blk is above otherBlk
			blk->VCGParents[id_oth] = otherBlk;
			otherBlk->VCGChildren[id_cur] = blk;
		} else if ( psRelLoc>0 && nsRelLoc>0 ) {
			// blk is on right of otherBlk
			blk->HCGParents[id_oth] = otherBlk;
			otherBlk->HCGChildren[id_cur] = blk;
		} else {
			// blk is below the otherBlk
			blk->VCGChildren[id_oth] = otherBlk;
			otherBlk->VCGParents[id_cur] = blk;
		} 
		
		
		// Edit source node for blk
		if(blk->HCGParents.size() == 0) {
			// add this node to the source children
			(*blocks)[SOURCE_ID]->HCGChildren[blk->id] = blk;
		} else {		
			(*blocks)[SOURCE_ID]->HCGChildren.erase(blk->id);
		}
		if(blk->VCGParents.size() == 0) {
			// add this node to the source children
			(*blocks)[SOURCE_ID]->VCGChildren[blk->id] = blk;
		} else {		
			(*blocks)[SOURCE_ID]->VCGChildren.erase(blk->id);
		}
		
		// Edit source node for otherBlk
		if(otherBlk->HCGParents.size() == 0) {
			// add this node to the source children
			(*blocks)[SOURCE_ID]->HCGChildren[otherBlk->id] = otherBlk;
		} else {		
			(*blocks)[SOURCE_ID]->HCGChildren.erase(otherBlk->id);
		}
		if(otherBlk->VCGParents.size() == 0) {
			// add this node to the source children
			(*blocks)[SOURCE_ID]->VCGChildren[otherBlk->id] = otherBlk;
		} else {		
			(*blocks)[SOURCE_ID]->VCGChildren.erase(otherBlk->id);
		}	
		
		
		
		// Edit target node for blk
		if(blk->HCGChildren.size() == 0) {
			// add this node to the source children
			(*blocks)[TARGET_ID]->HCGParents[blk->id] = blk;
		} else {		
			(*blocks)[TARGET_ID]->HCGParents.erase(blk->id);
		}
		if(blk->VCGChildren.size() == 0) {
			// add this node to the source children
			(*blocks)[TARGET_ID]->VCGParents[blk->id] = blk;
		} else {		
			(*blocks)[TARGET_ID]->VCGParents.erase(blk->id);
		}
		
		// Edit target node for otherBlk
		if(otherBlk->HCGChildren.size() == 0) {
			// add this node to the source children
			(*blocks)[TARGET_ID]->HCGParents[otherBlk->id] = otherBlk;
		} else {		
			(*blocks)[TARGET_ID]->HCGParents.erase(otherBlk->id);
		}
		if(otherBlk->VCGChildren.size() == 0) {
			// add this node to the source children
			(*blocks)[TARGET_ID]->VCGParents[otherBlk->id] = otherBlk;
		} else {		
			(*blocks)[TARGET_ID]->VCGParents.erase(otherBlk->id);
		}		
		
	}
}



void snSP::buildCG()
{
	snBlock *curBlk, *otherBlk;		// used in iteration
	for(int id_cur=0; id_cur<noOfBlocks; id_cur++) {
		curBlk = (*blocks)[id_cur];
		// a block is picked and compared with all other
		for(int id_oth=id_cur+1; id_oth<noOfBlocks; id_oth++) {
			otherBlk = (*blocks)[id_oth];

			// create an edge based on relative location
			int psRelLoc = curBlk->posSeqIndx - otherBlk->posSeqIndx;
			int nsRelLoc = curBlk->negSeqIndx - otherBlk->negSeqIndx;
			if( psRelLoc<0 && nsRelLoc<0 ) {
				// curBlk is on left of otherBlk
				curBlk->HCGChildren[id_oth] = otherBlk;
				otherBlk->HCGParents[id_cur] = curBlk; 
			} else if ( psRelLoc<0 && nsRelLoc>0 ) {
				// curBlk is above otherBlk
				curBlk->VCGParents[id_oth] = otherBlk;
				otherBlk->VCGChildren[id_cur] = curBlk;
			} else if ( psRelLoc>0 && nsRelLoc>0 ) {
				// curBlk is on right of otherBlk
				curBlk->HCGParents[id_oth] = otherBlk;
				otherBlk->HCGChildren[id_cur] = curBlk;
			} else {
				// curBlk is below the otherBlk
				curBlk->VCGChildren[id_oth] = otherBlk;
				otherBlk->VCGParents[id_cur] = curBlk;
			}
			
		}
	} 
	
		
	// update the CG pointers
	for(auto &blk : *blocks){
		for(auto &p : blk.second->HCGParents){
			p.second = (*blocks)[p.first];
		}
		for(auto &p : blk.second->HCGChildren){
			p.second = (*blocks)[p.first];
		}
		for(auto &p : blk.second->VCGParents){
			p.second = (*blocks)[p.first];
		}
		for(auto &p : blk.second->VCGChildren){
			p.second = (*blocks)[p.first];
		}
	}
}




void snSP::updateLocations()
{
	// set the initial location of the blocks
	//cout << "In updateLocations() " << blocks->size() << endl;
	for(auto &blkp : (*blocks)) {
		//cout << blkp.first << endl;
		blkp.second->locX = 0;
		blkp.second->locY = 0;
	}
	//cout << "CalcLong Check: 1" << endl;

	// update the x locations according to longest path from source
	int *order = new int[noOfBlocks];						// place to save the order
	topologicalOrder(HCG_TYPE, order);	// get the topological order
	//printArray(order, blocks->size());
	//cout << "CalcLong Check: 2" << endl;
	snBlock *thisBlk;		// current block
	snBlock *prntBlk;		// parent block 
	for(int i=0; i<noOfBlocks; i++) {
		//cout << "cur block: " << order[i] << " (" << i << ")" << endl;
		thisBlk = (*blocks)[order[i]];		// pick blocks in the order
		for(auto &blkp : thisBlk->HCGParents) {
			// fetch the parent block
			//cout << "CalcLong Check: 3  id:" << blkp.first << endl;
			if(blkp.first>=0) prntBlk = blkp.second;
			else continue;
			//cout << "CalcLong Check: 4" << endl;
			// if this path is larger, then update the location
			double &locX = thisBlk->locX;
			double newLoc = prntBlk->locX + prntBlk->width;
			if(locX < newLoc)  locX = newLoc;
			//cout << "CalcLong Check: 5" << endl;
		} 
		
	}
	// update the y locations according to longest path from source
	//cout << "CalcLong Check: 6" << endl;
	topologicalOrder(VCG_TYPE, order);	// get the topological order
	//cout << "CalcLong Check: 7" << endl;
	for(int i=0; i<noOfBlocks; i++) {
		thisBlk = (*blocks)[order[i]];		// pick blocks in the order
		for(auto &blkp: thisBlk->VCGParents) {
			// fetch the parent block
			if(blkp.first>=0) prntBlk = blkp.second;
			else continue;
			double &locY = thisBlk->locY;
			double newLoc = prntBlk->locY + prntBlk->height;
			// if this path is larger, then update the location
			if(locY < newLoc)  locY = newLoc;
		} 
		
	}
	
	
	// update the chip width and height
	snBlock *target = (*blocks)[TARGET_ID];
	for(auto &blkp: target->HCGParents) {
		prntBlk = blkp.second;
		double newLoc = prntBlk->locX + prntBlk->width;
		// if this path is larger, then update the location
		if( target->locX < newLoc)  target->locX = newLoc;
	} 
	for(auto &blkp: target->VCGParents) {
		prntBlk = blkp.second;
		double newLoc = prntBlk->locY + prntBlk->height;
		// if this path is larger, then update the location
		if( target->locY < newLoc)  target->locY = newLoc;
	} 
	width = target->locX;
	height = target->locY;
	area = width*height;	
}





// Variables for recursion
list<int> 		tpStack;		// stack for topological sorting
graphType_t 	tpGraphType;	// graph type to operate on
// Calculates and returs the topological order of all the successor of the
// source node in the block graph
void snSP::topologicalOrder(graphType_t gt,  int *order)
{
	// set the variables for recursion
	tpGraphType = gt;

	// Clear the necessary variables
	for(auto &blkp : *blocks) blkp.second->visited = false;	// clear the visited property
	tpStack.clear();

	topoSort((*blocks)[SOURCE_ID]);

	// create the order from the stack
	int i=0;
	while(!tpStack.empty()) {
		order[i++] = tpStack.back();	// save the last value
		tpStack.pop_back();				// delete the last value
	} 
}



// populates the stack in reverse topological order
void snSP::topoSort(snBlock* node)
{
	node->visited = true;	// mark the node
	//cout << node->id << " ";

	// select the proper children based on graph type
	unordered_map<int,snBlock*> *children;
	if(tpGraphType == HCG_TYPE)	children = &node->HCGChildren;
	else						children = &node->VCGChildren;

	// recurse and store in stack
	for(auto &blkp : *children) {
		if(blkp.first>=0 && !(blkp.second->visited)){ 	// id < 0 means target node
			topoSort(blkp.second); 
		}
	}
	if(node->id>=0)								// to skip the source
		tpStack.push_back(node->id);			// push in the stack 
}



void snSP::storeSnapShot(string filename, bool label)
{	
	genGnuPlot(blocks, snapShotBoundary, snapShotBoundary, filename.c_str(), label);
}



void snSP::rebuild()
{
	for(int i=0; i<noOfBlocks; i++) {
		// reload the sequence indices
		(*blocks)[posSeq[i]]->posSeqIndx = i;
		(*blocks)[negSeq[i]]->negSeqIndx = i;
		
		// clear the CG		
		snBlock *blk = (*blocks)[i];
		blk->HCGParents.clear();
		blk->HCGChildren.clear();
		blk->VCGParents.clear();
		blk->VCGChildren.clear();
		if(blk->rotation != blk->bestRotation)		
			blk->rotate();
	}
	
	snBlock *source = (*blocks)[SOURCE_ID];
	source->VCGChildren.clear();
	source->HCGChildren.clear();
	snBlock *target = (*blocks)[TARGET_ID];
	target->HCGParents.clear();
	target->VCGParents.clear();
	
	// build the CG
	buildCG();
	for(int i=0; i<noOfBlocks; i++) {
		snBlock *blk = (*blocks)[i];
		if(blk->HCGParents.size()==0) source->HCGChildren[i] = blk;
		if(blk->VCGParents.size()==0) source->VCGChildren[i] = blk;
		if(blk->HCGChildren.size()==0) target->HCGParents[i] = blk;
		if(blk->VCGChildren.size()==0) target->VCGParents[i] = blk;
	}
	updateLocations();
}






//------------ snFurnace Member functions ----------------
snMove& snFurnace::generateMove(int range)
{
	snSP &sp = *SP;
	int loc1   = rand() % sp.noOfBlocks;	// get an index
	int offset = rand() % range + 1;		// non zero offset from loc1 within range
	int loc2   = loc1 + offset;				// get another index
	// handle the border case
	//cout << loc1 << ": ";
	int shift;
	if (loc2 >= sp.noOfBlocks) {
		shift = loc2 % sp.noOfBlocks + 1;	
		loc1 -= shift;
		loc2 -= shift;	
	}
	

	move.type = rand() % MOVE_CNT;	
	switch(move.type) {
	case swap_ps:
		move.blk1_id = sp.posSeq[loc1];
		move.blk2_id = sp.posSeq[loc2];
		break;

	case swap_ns:
		move.blk1_id = sp.negSeq[loc1];
		move.blk2_id = sp.negSeq[loc2];
		break;

	case rotate:
		move.blk1_id = loc1;
		move.blk2_id = loc2;	// blk2_id is used for index access in some places
		break;
	}
	//cout << loc1 << " " << offset << " " << loc2 << endl;	
	return move;
}



void snFurnace::metropolis()
{
	double 	curCost, newCost, diffCost;
	double 	acceptThres;
	double 	aspRatio;
	bool	aspLegal;					// whether the aspect ration is legal
	int 	range;
	range = T/initTemp * (SP->noOfBlocks-2) + 1;	// some relation with temperature
	//cout << "range: " << range << endl;

	// for showing regular status
	time_t interval, end;
	interval = 0;

	//ofstream file("outputs/progress.gnu");
	//file << "# iteration  area" << endl;
	//cout << "Metropolis: Starting iteration" << endl;
	for(int i=0; i<M; i++) { 
		curCost = SP->area;
		generateMove(range);			// move operates on private variable "move"
		SP->applyMove(move);
		SP->updateLocations();

		newCost = SP->area;
		aspRatio = SP->width/SP->height;

		diffCost = newCost - curCost;
		acceptThres = (double)rand() / RAND_MAX;	// get a number 0<=x<=1

		aspLegal = (aspRatio>=aspRatioMin && aspRatio<=aspRatioMax);
		if( (diffCost<0 || acceptThres < exp(-1*diffCost/T)) && (!considerAspRatio || aspLegal)) {
			// accept
			
			if(SP->area < bestSP.area) {
				bestSP.clone(*SP);			// save the best result
				for(auto &blkp: *(SP->blocks))
					blkp.second->bestRotation = blkp.second->rotation;
			}
		}else {
			// reject
			move.invert();
			SP->applyMove(move);
			SP->updateLocations();
		}



		time(&end);
		interval = end - strInt;
		wallClock = end - strWall;
		//cout << interval << endl;
		if(interval >= INFO_INTERVAL){
			showStatus();
			cout << "\n\n";
			time(&strInt);
		}
	}
	//cout << "Metropolis: Ending iteration\n\n\n"; 
}


void snFurnace::loadDefaults()
{
	alpha 	= 0.9;
	beta 	= 0.9;
	T 		= 2;
	Tmin 	= T/SP->noOfBlocks;
	aspRatioMax 	= 1.5;
	aspRatioMin 	= 0.6;
	snapShotCount 	= 10;
	maxTime 		= 200;	
	M = 10*SP->noOfBlocks;	
	snapLabel = false;
	pltLoc = "../outputs/output.plt";
}


void snFurnace::anneal()
{
	bestSP.clone(*SP);
	initTemp = T;		// save the initial temperature
	considerAspRatio = false;		// initially don't consider
	plotInfo = "";
	double saveTempInt = (T-Tmin)/snapShotCount;
	double saveTemp = T-saveTempInt;
	string tempName;

	// set the timers
	wallClock = 0;
	time(&strWall);
	time(&strInt);

	// initialize the variables related to snap shots
	snapNo = 0;
	snapLoc = SNAP_LOC;
	tempName = "../outputs/init.gnu";
	SP->storeSnapShot(tempName, snapLabel);
	plotInfo = "  " + to_string(snapNo) \
								+ "\t" + to_string(T) + "\t" \
								+ to_string(SP->area) + "\t" \
								+ (tempName+".png\t") \
								+ to_string(SP->width/SP->height) + "\t" \
								+ to_string(SP->cellArea/SP->area*100) + "%\t" \
								+ to_string(wallClock) + "\n";
	snapNo++;

	//cout << "Starting Annealing..." << endl;
	bool brkFlag = false;
	while(!brkFlag) {

		if(wallClock >= maxTime) {
			cout << "Annealing Stopped: Maximum Time limit reached" << endl;
			brkFlag = true;
			break;
		}
		if( (Tmin>0) && (T <= Tmin)) {
			cout << "Annealing Stopped: Furnace is cold" << endl;
			brkFlag = true;
			break;
		} 

		metropolis();
		T = alpha * T;					// temperature cooling
		M = beta * M;					// move reduction
		if(T<saveTemp) {
			tempName = snapLoc + to_string(snapNo) + ".gnu";
			SP->storeSnapShot( tempName, snapLabel);
			plotInfo = plotInfo + "  " + to_string(snapNo) \
								+ "\t" + to_string(T) + "\t" \
								+ to_string(SP->area) + "\t" \
								+ (tempName+".png\t") \
								+ to_string(SP->width/SP->height) + "\t" \
								+ to_string(SP->cellArea/SP->area*100) + "%\t" \
								+ to_string(wallClock) + "\n";
			saveTemp -= saveTempInt;
			snapNo++;
		}
		
		
	}
	showStatus();
}



void snFurnace::showStatus()
{
	cout << "# Furnace Status\n";
	cout << "Temp: " << T << endl;
	cout << "Time: " << wallClock << " sec" << endl; 
	cout << "Area: " << bestSP.area << endl;
	cout << "Dim : (" << bestSP.width << ", " << bestSP.height << ")" << endl;
	cout << "Aspect Ratio: " << bestSP.width/bestSP.height << endl;
	cout << "Utilization : " << bestSP.cellArea/bestSP.area * 100 << "%" << endl; 
}


void snFurnace::genPlt()
{
	// plot file
	ofstream file(pltLoc);
	if(!file) {
		cout << "Failed to open file: " << pltLoc << endl;
		exit(1);
	}
	plotInfo = plotInfo + "  " + to_string(snapNo) \
						+ "\t" + to_string(T) + "\t" \
						+ to_string(bestSP.area) + "\t" \
						+ ("../outputs/best.gnu.png\t") \
						+ to_string(bestSP.width/bestSP.height) + "\t" \
						+ to_string(bestSP.cellArea/bestSP.area*100) + "%\t" \
						+ to_string(wallClock) + "\n";
	file << PLT_HEADER;
	file << snapNo << "   " << snapNo << "\n\n\n";
	file << COL_NAMES << endl;
	file << plotInfo;
	cout << "Plot information saved" << endl; 
}





