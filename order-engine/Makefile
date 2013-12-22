CXXFLAGS = -std=c++0x

OBJS = main.o Order.o

INCLUDES = -Iinclude

LFLAGS = -Llibs

JSONLIB = -ljson_linux-gcc-4.6_libmt

GTESTLIB = -lgtest


all: $(OBJS)
	g++ -o order-engine $(OBJS) $(CXXFLAGS) $(INCLUDES) $(LFLAGS) $(JSONLIB)

tests: tests.cpp Order.o
	g++ -o test-engine $(CXXFLAGS) Order.o -I. $(INCLUDES) -pthread tests.cpp $(LFLAGS) $(GTESTLIB) $(JSONLIB)

Order.o: Order.cpp Order.h
	g++ -o Order.o -c $(CXXFLAGS) $(INCLUDES) Order.cpp

main.o: main.cpp
	g++ -o main.o -c $(CXXFLAGS) $(INCLUDES) main.cpp
	
clean:
	rm -f *.o order-engine test-engine
