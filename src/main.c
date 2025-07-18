#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <gelf.h>
#include <libelf.h>

#include "decode.h"

// flow: main.c -> decode.c -> a32_tables.c -> instr_*.c 

uint64_t curr_addr;
uint32_t curr_instr;

void fatal(const char *msg) {
    perror(msg);
    exit(1);
}

// Find .text section with libelf
uint8_t *getTextSection(const char *elf_fname, size_t *ptext_size, uint64_t *ptext_addr) {
    // check libelf version
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fatal("elf_version");
    }

    // open ELF file
    int fd = open(elf_fname, O_RDONLY);
    if (fd < 0) {
        fatal("open");
    }

    Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf) {
        fatal("elf_begin");
    }

    // get section header string table index
    size_t sh_str_idx;
    if (elf_getshdrstrndx(elf, &sh_str_idx) != 0) {
        fatal("elf_getshdrstrndx");
    }

    Elf_Scn *section = NULL;
    GElf_Shdr shdr; // section header
    char *name;

    // Iterate sections to find ".text"
    while ((section = elf_nextscn(elf, section)) != NULL) { // gte section
        if (gelf_getshdr(section, &shdr) != &shdr) {    // get section header for section
            fatal("gelf_getshdr");
        }

        name = elf_strptr(elf, sh_str_idx, shdr.sh_name);
        if (name && strcmp(name, ".text") == 0) {   // found .text section
            *ptext_addr = (uint64_t)shdr.sh_addr;
            *ptext_size = (size_t)shdr.sh_size;
            break;
        }
    }

    if (!section) {
        fprintf(stderr, ".text section not found\n");
        elf_end(elf);
        close(fd);
        exit(1);
    }

    // get section data
    Elf_Data *data = elf_getdata(section, NULL);
    if (!data) {
        fatal("elf_getdata");
    }

    // copy into separate buffer so we can close the file
    uint8_t *text_buf = malloc(data->d_size);
    if (!text_buf) {
        fatal("malloc");
    }
    memcpy(text_buf, data->d_buf, data->d_size);

    printf(".text section found:\n");
    printf("    Address: 0x%lx\n", *ptext_addr);
    printf("    Size:    0x%lx\n", *ptext_size);

    elf_end(elf);
    close(fd);

    return text_buf;
}

// breakarm A32 disassembler implementation
void breakarmDisas(uint8_t *text_buf, size_t text_size, uint64_t text_addr) {
    uint64_t start_addr = text_addr;
    uint8_t a32_instr_size = 4;
    //uint64_t end_addr = text_addr + text_size;
    uint32_t num_instr = text_size / a32_instr_size;

    curr_addr = start_addr;
    printf("\n%s\t%s\t\t%s\n", "ADDR", "INSTR", "ASSEMBLY");
    for (uint32_t i = 0; i < num_instr; i++) {
        // address, instruction
        curr_instr = *((uint32_t *)text_buf + i);
        printf("0x%lx:\t0x%08x\t", curr_addr, curr_instr);
        // assembly code
        decode_instr(curr_instr); // also prints

        curr_addr += a32_instr_size;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF binary>\n", argv[0]);
        return 1;
    }

    char *elf_fname = argv[1];
    size_t text_size;
    uint64_t text_addr;

    // note text_buf was malloced
    uint8_t *text_buf = getTextSection(elf_fname, &text_size, &text_addr);


    // breakarm
    breakarmDisas(text_buf, text_size, text_addr);

    
    free(text_buf);

    return 0;
}