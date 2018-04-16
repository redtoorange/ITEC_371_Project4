CXX = g++
CXXFLAGS = -std=c++11 

OBJECTS =   FSObject.o Directory.o File.o Util.o TextFile.o  ProgramFile.o Process.o Scheduler.o main.o

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -static-libstdc++ -o RUIN main.o FSObject.o Directory.o File.o Util.o TextFile.o ProgramFile.o Process.o Scheduler.o

ProgramFile.o:
	g++ $(CXXFLAGS) -c ProgramFile.cpp

Process.o:
	g++ $(CXXFLAGS) -c Process.cpp

Scheduler.o:
	g++ $(CXXFLAGS) -c Scheduler.cpp

Util.o:
	g++ $(CXXFLAGS) -c Util.cpp

FSObject.o:
	g++ $(CXXFLAGS) -c FSObject.cpp

Directory.o:
	g++ $(CXXFLAGS) -c Directory.cpp

File.o:
	g++ $(CXXFLAGS) -c File.cpp

TextFile.o:
	g++ $(CXXFLAGS) -c TextFile.cpp

main.o:
	g++ $(CXXFLAGS) -c main.cpp
