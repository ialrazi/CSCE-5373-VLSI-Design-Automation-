#ifndef DATASTRUCT_H
#define DATASTRUCT_H


#include <vector>
#include <unordered_map>
#include <ctime>
#include <string>
#include "Defs.h"


class snBlock {
public:
	// Basic information
	int 	id;						// a unique ID
	double 	width, height;			// dimension
	bool 	rotation;				// is rotated?
	double 	locX, locY;				// location

	// Sequence pair information
	int 	posSeqIndx, negSeqIndx;	// index in the Sequence Pair

	// Graph Information
	std::unordered_map<int,snBlock*> VCGParents;	// Parents in vertical constraint graph
	std::unordered_map<int,snBlock*> VCGChildren;	// Children in vertical constraint graph
	std::unordered_map<int,snBlock*> HCGParents;	// Parents in horizontal constraint graph
	std::unordered_map<int,snBlock*> HCGChildren;	// Children in horizontal constraint graph
	bool visited;

	// information related to best result
	bool bestRotation;
	
public:
	void printInfo();
	void rotate();
};





class snMove {
public:
	int type;
	int blk1_id;
	int blk2_id;

public:
	void printInfo();
	void invert();
};





class snSP {
public:
	int noOfBlocks;									// no of blocks
	std::vector<int> posSeq;						// Positive sequence
	std::vector<int> negSeq;						// Negative sequence
	double cellArea;								// area occupied by cells
	double area;									// area of this sp
	double width;
	double height;
	double snapShotBoundary;						// Boundary of the GNU plot
	
	std::unordered_map<int,snBlock*> *blocks;		// Pointer to blocks database

public:
	void printInfo();
	void clear();
	void clone(snSP &ob);
	void applyMove(snMove &move);
	void buildCG();
	void rebuild();
	void updateLocations();
	void storeSnapShot(std::string filename, bool label=true);
	
	// should be private methods
	
private:	
	void updateSubGraph		(snBlock *blk, std::vector<int> &nodesID);
	void topologicalOrder	(graphType_t gt,  int *order);
	void topoSort			(snBlock* node);
};



class snFurnace {
private:
//public:
	double 		initTemp;
	std::time_t wallClock, strInt, strWall;
	snMove 		move;
	bool 		considerAspRatio;	// whether or not consider aspect ratio
	std::string	snapLoc;
	std::string plotInfo;
	int snapNo;

public:
	double alpha;			// cooling rate
	double beta;			// time increase rate
	double T;				// termperature
	double Tmin;			// minimum termperature to stop
	double M;				// No. of iteration
	double aspRatioMax;		// 
	double aspRatioMin;
	int    snapShotCount;	// No. of snapshots to save
	bool   snapLabel;	
	std::string pltLoc;			// Location to save the plot file
	std::time_t maxTime;	// Maximum Run time in seconds
	snSP *SP;
	snSP bestSP;

private:
	snMove& generateMove(int range);
	void 	metropolis();
	void 	showStatus();
	
public:
	void loadDefaults();
	void anneal();
	void genPlt();
	
};


#endif //DATASTRUCT_H



