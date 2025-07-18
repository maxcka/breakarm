CC_ARM32 = arm-linux-gnueabi-gcc
CFLAGS = -Wall -Wextra -g3
SRC = disasm_test.c
OBJ = $(SRC:.c=.o)

all: $(OBJ)


$(OBJ): $(SRC)
	$(CC_ARM32) $(CFLAGS) -marm -o $@ -c $^


clean:
	rm -f $(OBJ) 