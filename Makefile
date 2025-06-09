CC = gcc
CFLAGS = -Wall -Wextra -g -I.
LDFLAGS = -lelf -lcapstone
SRC = main.c decode.c decode_utils.c a32_tables.c instr_data_proc.c
OBJ = $(SRC:.c=.o)
PROG = arm32_disasm

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJ)

