CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2
TARGET = epollserver
LIB = libepollserver.a
SRCS = Main.cc
OBJS = Main.o

all: $(TARGET) $(LIB)

$(TARGET): $(OBJS) $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(LIB): $(OBJS)
	ar rcs $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(LIB)

.PHONY: all clean