CXX = g++
CXXFLAGS = -O3 -Wall -std=c++14 $(shell root-config --cflags)
LDFLAGS = $(shell root-config --libs)

TARGET = ising_sim
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET) Ising_Simulation_Results.pdf Ising_Simulation_Results.png
