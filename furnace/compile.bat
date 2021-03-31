g++ -std=c++11 -c helperFuncs.cpp -o helperFuncs.o 
g++ -std=c++11 -c DataStruct.cpp -o DataStruct.o 
g++ -std=c++11 -c main.cpp -o main.o
g++ -std=c++11 main.o DataStruct.o helperFuncs.o -o furnace.exe





