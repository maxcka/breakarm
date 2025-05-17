#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <gelf.h>
#include <libelf.h>



void fatal(const char *msg) {
    perror(msg);
    exit(1);
}

// Find .text section
void findTextSection() {

}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF binary>\n", argv[0]);
        return 1;
    }

    char *elf_fname = argv[1];

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
    while ((section = elf_nextscn(elf, section)) != NULL) {
        if (gelf_getshdr(section, &shdr) != &shdr) {
            fatal("gelf_getshdr");
        }

        name = elf_strptr(elf, sh_str_idx, shdr.sh_name);
        if (name && strcmp(name, ".text") == 0) {   // found .text section
            break;
        }
    }

    if (!section) {
        fprintf(stderr, ".text section not found\n");
        elf_end(elf);
        close(fd);
        return 1;
    }

    // get section data
    Elf_Data *data = elf_getdata(section, NULL);
    if (!data) {
        fatal("elf_getdata");
    }

    printf(".text section found:\n");
    printf("    Address: 0x%lx\n", (unsigned long)shdr.sh_addr);
    printf("    Size:    0x%lx\n", (unsigned long)shdr.sh_size);

    elf_end(elf);
    close(fd);
    return 0;


    return 0;
}