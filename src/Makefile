CC = gcc
CFLAGS = -Wall -Wextra -g -I../include
LDFLAGS = -lelf -lcapstone
SRC = main.c decode.c decode_utils.c a32_tables.c \
		instr/instr_data_proc.c \
		instr/instr_misc.c \
		instr/instr_half_mult.c \
		instr/instr_mult.c \
		instr/instr_sync.c
OBJ = $(SRC:.c=.o)
PROG = arm32_disasm

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJ)

