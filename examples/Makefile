#CC_AMD64 = gcc
CC_ARM32 = arm-linux-gnueabi-gcc
#CC_ARM64 = aarch64-linux-gnu-gcc
CFLAGS = -Wall -Wextra -g3
#LDFLAGS = 
SRC = test.c
#OBJ = $(SRC:.c=.o)
PROG_AMD64 = test_x86_64
PROG_ARM32 = test_arm32
PROG_ARM64 = test_arm64
PROG_THUMB2 = test_thumb2

all: $(PROG_ARM32) make_tests #$(PROG_AMD64) $(PROG_ARM64) $(PROG_THUMB2)

#$(PROG_AMD64): $(SRC)
#	$(CC_AMD64) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(PROG_ARM32): $(SRC)
	$(CC_ARM32) $(CFLAGS) -marm -o $@ $^ $(LDFLAGS)

#$(PROG_ARM64): $(SRC)
#	$(CC_ARM64) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#$(PROG_THUMB2): $(SRC)
#	$(CC_ARM32) $(CFLAGS) -mthumb -o $@ $^ $(LDFLAGS)

make_tests:
	$(MAKE) -C tests

clean:
	rm -f $(PROG_ARM32) 
	$(MAKE) -C tests clean
	
#$(PROG_AMD64) $(PROG_ARM64) $(PROG_THUMB2)