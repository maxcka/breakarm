CC_ARM32 = arm-linux-gnueabi-gcc
CFLAGS = -Wall -Wextra -g3
#LDFLAGS = 
SRC = example1.c
#OBJ = $(SRC:.c=.o)
PROG_ARM32 = example1

all: $(PROG_ARM32)

$(PROG_ARM32): $(SRC)
	$(CC_ARM32) $(CFLAGS) -marm -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROG_ARM32) 