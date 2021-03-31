#ifndef HELPERFUNC_H
#define HELPERFUNC_H

#include <unordered_map>



// function prototypes
bool readSPG 			(const char *filename, std::unordered_map<int,snBlock*> &blocks, snSP &sp);
void printBlocks		(std::unordered_map<int,snBlock*> *blocks);
void genGnuPlot			(std::unordered_map<int,snBlock*> *blocks, int width, int height, const char *filename, bool label=true);
void randColor			(char *color1, char *color2);
void saveFP				(std::unordered_map<int,snBlock*> *blocks, const char *filename);


#endif //HELPERFUNC_H

