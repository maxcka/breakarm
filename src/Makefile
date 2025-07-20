CC = gcc
CFLAGS = -Wall -Wextra -g -I../include
LDFLAGS = -lelf
SRC = main.c decode.c decode_utils.c a32_tables.c \
		instr/instr_default.c \
		instr/instr_data_proc.c \
		instr/instr_misc.c \
		instr/instr_half_mult.c \
		instr/instr_mult.c \
		instr/instr_sync.c \
		instr/instr_load_store.c \
		instr/instr_parallel_add_sub.c \
		instr/instr_pack_sat_rev.c \
		instr/instr_branch_block.c \
		instr/instr_coproc.c \
		instr/instr_uncond.c
OBJ = $(SRC:.c=.o)
PROG = breakarm

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJ)

