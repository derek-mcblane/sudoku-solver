OPTS = -std=c++11
LIBS = 

all: solver

DEBUG_LIBS = -lpthread 
debug: OPTS += -g -DDEBUG
debug: LIBS += $(DEBUG_LIBS)
debug: all

solver: main.o
	g++ main.o -o solver $(OPTS) $(LIBS)

main.o: main.cpp
	g++ -c main.cpp -o main.o $(OPTS) $(LIBS)

clean:
	rm *.o; rm solver;

