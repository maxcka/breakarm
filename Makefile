CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lelf -lcapstone
SRC = main.c
OBJ = $(SRC:.c=.o)
PROG = arm32_disasm

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJ)

