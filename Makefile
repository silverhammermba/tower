CXXFLAGS=-std=c++11 -ggdb -pg

tower: main.o
	$(CXX) $(CXXFLAGS) -o $@ $< -lGL -lGLU -lGLEW -lSDL2 -lSDL2_image

main.o: main.cpp *.hpp

clean:
	rm -f tower *.o
