CXXFLAGS=-std=c++11 -ggdb -pg

tower: main.o
	$(CXX) $(CXXFLAGS) -o $@ $+ -lGL -lGLEW -lSDL2 -lSDL2_image
