
CXX = g++
CXXFLAGS = -Wall -std=c++23
TARGETS = main
SRC_MAIN = ../src/main_server.cpp ../lib/libtslog.cpp

all: $(TARGETS)

main: $(SRC_MAIN)
	$(CXX) $(CXXFLAGS) -o main $(SRC_MAIN)

clean:
	del $(TARGETS)