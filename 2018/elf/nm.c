#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void* xmalloc(size_t num_bytes) {
    char *ptr = malloc(num_bytes);
    if (!ptr) {
        perror("xmalloc failed");
        exit(1);
    }
    return ptr;
}

size_t read_file(char **buf, const char *path) {
    *buf = NULL;
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *buf = xmalloc(len+1);
    if (len && fread(*buf, len, 1, fp) != 1) {
        fclose(fp);
        free(*buf);
        return 0;
    }
    (*buf)[len] = '\0';
    fclose(fp);
    return len;
}

int name_compare(const void *x, const void *y, void *arg) {
    Elf64_Sym * const *sx = x;
    Elf64_Sym * const *sy = y;
    char *strtab = arg;

    const char *x_name = strtab + (*sx)->st_name;
    const char *y_name = strtab + (*sy)->st_name;

    // Ignore leading underscores.
    while (*x_name == '_')
        x_name++;
    while(*y_name == '_')
        y_name++;
    return strcasecmp(x_name, y_name);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "USAGE: nm OBJFILE\n");
        exit(1);
    }

    const char *path = argv[1];
    char *buf = NULL;

    size_t len = read_file(&buf, path);
    if (!len) {
        fprintf(stderr, "could not read from file\n");
        exit(1);
    }

#define SECTION_HDR(index) ((Elf64_Shdr*)(buf+ehdr->e_shoff + (index) * ehdr->e_shentsize))

    // There are lots and lots of checks that are left out in the code below.
    // Production code would have to verify that all offsets lies within the
    // size of the file and that all fields have legitimate values.
    //
    // The parsing is done by casting structs over the file buffer. This
    // assumes that the allocated memory for the buffer is properly aligned;
    // that the structs does not contain any padding; that the structs start
    // addresses are properly aligned; and that the field within the structs
    // are aligned.
    //
    // All this holds for a native ELF-file, but it may not be true for ELFS
    // compiled for other platforms than the host.
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)buf;

    // Find the string table, .shstrtab, containing the strings for the section names.
    char *shstrtab = buf + SECTION_HDR(ehdr->e_shstrndx)->sh_offset;

    Elf64_Shdr *symtab_hdr = NULL;
    char *strtab = NULL;

    // Find the symbol table, .symtab, and its associated string table .strtab.
    for (int i = 0 ; i < ehdr->e_shnum; i++) {
        Elf64_Shdr *sh = SECTION_HDR(i);
        const char* name = shstrtab + sh->sh_name;

        if (strcmp(".symtab", name) == 0) {
            symtab_hdr = SECTION_HDR(i);
        } else if (strcmp(".strtab", name) == 0) {
            strtab = buf + sh->sh_offset;
        }
    }

    if (!symtab_hdr || !strtab) {
        fprintf(stderr, "no symbols");
        exit(1);
    }

    // Sort the pointers to the symbols by name.
    size_t num_symbols = symtab_hdr->sh_size / sizeof(Elf64_Sym);
    Elf64_Sym **symbols = xmalloc(num_symbols * sizeof(Elf64_Sym));
    for (size_t i = 0; i < num_symbols; i++) {
        symbols[i] = (Elf64_Sym*)(buf + symtab_hdr->sh_offset + (i * sizeof(Elf64_Sym)));
    }
    qsort_r(symbols, num_symbols, sizeof(symbols[0]), name_compare, strtab);

    // Classify the symbol types.
    // A simplistic algorithm. GNU nm has far more involved checks. In
    // particular, we don't check for weak linkage.
    for (size_t i = 0; i < num_symbols; i++) {
        Elf64_Sym* sym = symbols[i];

        char type = ' ';
        if (sym->st_shndx > 0 && sym->st_shndx < ehdr->e_shnum) {
            Elf64_Shdr *sh = SECTION_HDR(sym->st_shndx);
            if (sh->sh_type == SHT_PROGBITS 
                    || sh->sh_type == SHT_INIT_ARRAY 
                    || sh->sh_type == SHT_FINI_ARRAY) {
                if ((sh->sh_flags & SHF_ALLOC) == 0) {
                    type = '?';
                } else if (sh->sh_flags & SHF_EXECINSTR) {
                    type = 't';
                } else if (sh->sh_flags & SHF_WRITE) {
                    type = 'd';
                } else {
                    type = 'r';
                }
            } else if (sh->sh_type == SHT_NOBITS) {
                type = 'b';
            }
        } else if (sym->st_shndx == SHN_UNDEF) {
            type = 'u';
        }

        if (ELF64_ST_BIND(sym->st_info) == STB_GLOBAL) {
            type = toupper(type);
        }

        // Don't print the address for undefined symbols.
        const char* name = strtab + sym->st_name;
        if (type != ' ' && *name) {
            if (type == 'u' || type == 'U') {
                printf("%16s %c %s\n", " ", type, name);
            } else {
                printf("%016lx %c %s\n", sym->st_value, type, name);
            }
        }
    }
    return 0;
}
