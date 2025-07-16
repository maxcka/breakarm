CC_ARM32 = arm-linux-gnueabi-gcc
CFLAGS = -Wall -Wextra -g3
#LDFLAGS = 
SRC = disasm_test.c
OBJ = $(SRC:.c=.o)
#PROG_ARM32 = $(SRC:.c=)

all: $(OBJ)


$(OBJ): $(SRC)
	$(CC_ARM32) $(CFLAGS) -marm -o $@ -c $^ $(LDFLAGS)


clean:
	rm -f $(PROG_ARM32) 