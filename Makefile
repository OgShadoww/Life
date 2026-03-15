CC = clang
TARGET = main
SRC = main.c

$(TARGET): Makefile $(SRC)
	$(CC) $(SRC) -o $(TARGET)
