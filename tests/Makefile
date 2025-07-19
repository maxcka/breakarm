CC_ARM32 = arm-linux-gnueabi-gcc
CFLAGS = -Wall -Wextra -g3
SRC = test_disasm.c
OBJ = $(SRC:.c=.o)

all: $(OBJ)

# needed to use -mcpu=cortex-a15 to be able to compile some instructions
# cortex-a15 has support for these instructions
$(OBJ): $(SRC)
	$(CC_ARM32) $(CFLAGS) -marm -mcpu=cortex-a15 -o $@ -c $^


clean:
	rm -f $(OBJ) 