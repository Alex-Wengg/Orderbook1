CC = g++
CFLAGS = -Wall -Wextra

SRCS = Main.cpp OrderBook.cpp OrderTree.cpp Order.cpp OrderList.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = Example.exe

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
