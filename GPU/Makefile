CC = gcc
CFLAGS = -O2 -Wall -Wno-unused-function
LDFLAGS = -lglut -lOpenCL
EXEC = raygpu
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean mrproper

clean:
		rm -rf $(OBJ) $(EXEC) *~ *.orig .*.swp
