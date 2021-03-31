

main: main.o DataStruct.o helperFuncs.o
	g++ -std=c++11 main.o DataStruct.o helperFuncs.o -o furnace.out

run_main: main
	@printf "\n\n--------------- Running Main ----------------\n\n"
	./furnace.out processor.cfg

main.o: main.cpp helperFuncs.o DataStruct.o
	g++ -std=c++11 -c main.cpp -o main.o


snTests2: snTests2.o DataStruct.o helperFuncs.o
	g++ -std=c++11 snTests2.o DataStruct.o helperFuncs.o  -o snTests2.out 
	@printf "\n\n------ Testing ------> "
	./snTests2.out


snTests2.o: snTests2.cpp helperFuncs.o DataStruct.o
	g++ -std=c++11 -c snTests2.cpp -o snTests2.o 

helperFuncs.o: helperFuncs.h helperFuncs.cpp
	g++ -std=c++11 -c helperFuncs.cpp -o helperFuncs.o 
	
DataStruct.o: DataStruct.h DataStruct.cpp
	g++ -std=c++11 -c DataStruct.cpp -o DataStruct.o 


clean:
	rm -f *.o
	rm -f *.out
	rm -f ../outputs/*.gnu ../outputs/*.png
