CXX = g++
CXXFLAGS = -Wall -g

MAIN = Example

SOURCES = ./Example.cpp ./Order.cpp ./OrderList.cpp ./OrderTree.cpp

HEADERS = ./Order.h ./OrderList.h ./OrderTree.h ./OrderBook.h

OBJECTS = $(SOURCES:.cpp=.o)

all: $(MAIN)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -v -c $< -o $@

$(MAIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -v -o $(MAIN) $(OBJECTS)

# Clean the build directory
clean:
	rm -f $(OBJECTS) $(MAIN)  *~