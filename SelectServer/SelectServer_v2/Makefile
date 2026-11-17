CC = g++
CFLAGS = -std=c++17 -Wall
TARGET = selectserver
SRC = Main.cc
HEADERS = $(wildcard *.hpp)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: clean
