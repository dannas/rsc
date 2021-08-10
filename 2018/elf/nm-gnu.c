#include <assert.h>
#include <ctype.h>
#include <elf.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)

void *xrealloc(void *ptr, size_t num_bytes) {
    ptr = realloc(ptr, num_bytes);
    if (!ptr) {
        perror("xrealloc failed");
        exit(1);
    }
    return ptr;
}


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

// Stretchy buffers, invented (?) by Sean Barrett

typedef struct BufHdr {
    size_t len;
    size_t cap;
    char buf[];
} BufHdr;

#define buf__hdr(b) ((BufHdr *)((char *)(b) - offsetof(BufHdr, buf)))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_end(b) ((b) + buf_len(b))

#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)
#define buf_fit(b, n) ((n) <= buf_cap(b) ? 0 : ((b) = buf__grow((b), (n), sizeof(*(b)))))
#define buf_push(b, ...) (buf_fit((b), 1 + buf_len(b)), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))
#define buf_clear(b) ((b) ? buf__hdr(b)->len = 0 : 0)

void *buf__grow(const void *buf, size_t new_len, size_t elem_size) {
    assert(buf_cap(buf) <= (SIZE_MAX - 1)/2);
    size_t new_cap = CLAMP_MIN(2*buf_cap(buf), MAX(new_len, 16));
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(BufHdr, buf))/elem_size);
    size_t new_size = offsetof(BufHdr, buf) + new_cap*elem_size;
    BufHdr *new_hdr;
    if (buf) {
        new_hdr = xrealloc(buf__hdr(buf), new_size);
    } else {
        new_hdr = xmalloc(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}

typedef struct Symbol {
    uint64_t address;
    char type;
    char *name;
} Symbol;

int name_compare(const void *x, const void *y) {
    const Symbol *sx = x;
    const Symbol *sy = y;

    const char *x_name = sx->name;
    const char *y_name = sy->name;

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
    char *buf;

    size_t len = read_file(&buf, path);
    if (!len) {
        fprintf(stderr, "could not read from file\n");
        exit(1);
    }
    if (len < sizeof(Elf64_Ehdr)) {
        fprintf(stderr, "Not enough room for ELF header\n");
        exit(1);
    }

    Elf64_Ehdr *hdr = (Elf64_Ehdr*)buf;

#if 0
    printf("Magic:   ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%.2X ", hdr->e_ident[i]);
    }
    printf("\n");
    printf("Start of program headers:           %ld (bytes into file)\n", hdr->e_phoff);
    printf("Start of section headers:           %ld (bytes into file)\n", hdr->e_shoff);
    printf("Size of this header:                %d (bytes)\n", hdr->e_ehsize);
    printf("Size of program headers:            %d (bytes)\n", hdr->e_phentsize);
    printf("Number of program headers:          %d (bytes)\n", hdr->e_phnum);
    printf("Size of section headers:            %d (bytes)\n", hdr->e_shentsize);
    printf("Number of section headers:          %d (bytes)\n", hdr->e_shnum);
    printf("Section header string table index:  %d\n", hdr->e_shstrndx);
#endif

#define SECTION_HDR(index) ((Elf64_Shdr*)(buf+hdr->e_shoff + (index) * hdr->e_shentsize))

    // The section header string table, .shstrtab
    // TODO(dannas): e_shstrndx may be undefined. Need to check for that
    Elf64_Shdr *sh = SECTION_HDR(hdr->e_shstrndx);
    char *shstrtab = buf + sh->sh_offset;
    size_t shstrtab_size = sh->sh_size;

    int symtab_index = -1;
    char* strtab = NULL;

    // Find the symbol table, .symtab, and its associated string table .strtab.
    for (int i = 0 ; i < hdr->e_shnum; i++) {
        Elf64_Shdr *sh = SECTION_HDR(i);
        uint16_t str_index = sh->sh_name;
        const char* name = shstrtab+str_index;

        //printf("i=%d sh_name=%d %s\n", i, str_index, name);

        if (strcmp(".symtab", name) == 0) {
            symtab_index = i;
        } else if (strcmp(".strtab", name) == 0) {
            strtab = buf + sh->sh_offset;
        }
    }

    // Print the symbols in the symbol table
    Elf64_Shdr *symtab_hdr = SECTION_HDR(symtab_index);

    Symbol *symbols = NULL;

    // TODO(dannas): Fix symbol name resolution with this function from binutils-gdb/bfd/nm.c

//int
//bfd_decode_symclass (asymbol *symbol)
//{
//  char c;
//
//  if (symbol->section && bfd_is_com_section (symbol->section))
//    return 'C';
//  if (bfd_is_und_section (symbol->section))
//    {
//      if (symbol->flags & BSF_WEAK)
//	{
//	  /* If weak, determine if it's specifically an object
//	     or non-object weak.  */
//	  if (symbol->flags & BSF_OBJECT)
//	    return 'v';
//	  else
//	    return 'w';
//	}
//      else
//	return 'U';
//    }
//  if (bfd_is_ind_section (symbol->section))
//    return 'I';
//  if (symbol->flags & BSF_GNU_INDIRECT_FUNCTION)
//    return 'i';
//  if (symbol->flags & BSF_WEAK)
//    {
//      /* If weak, determine if it's specifically an object
//	 or non-object weak.  */
//      if (symbol->flags & BSF_OBJECT)
//	return 'V';
//      else
//	return 'W';
//    }
//  if (symbol->flags & BSF_GNU_UNIQUE)
//    return 'u';
//  if (!(symbol->flags & (BSF_GLOBAL | BSF_LOCAL)))
//    return '?';
//
//  if (bfd_is_abs_section (symbol->section))
//    c = 'a';
//  else if (symbol->section)
//    {
//      c = coff_section_type (symbol->section->name);
//      if (c == '?')
//	c = decode_section_type (symbol->section);
//    }
//  else
//    return '?';
//  if (symbol->flags & BSF_GLOBAL)
//    c = TOUPPER (c);
//  return c;
//
//  /* We don't have to handle these cases just yet, but we will soon:
//     N_SETV: 'v';
//     N_SETA: 'l';
//     N_SETT: 'x';
//     N_SETD: 'z';
//     N_SETB: 's';
//     N_INDR: 'i';
//     */
//}

    for (int i = 0; i < symtab_hdr->sh_size; i += sizeof(Elf64_Sym)) {
        Elf64_Sym* sym = (Elf64_Sym*)(buf + symtab_hdr->sh_offset + i);

        int binding = ELF64_ST_BIND(sym->st_info);

        char c = ' ';
        if (sym->st_shndx > 0 && sym->st_shndx < hdr->e_shnum) {
            Elf64_Shdr *sh = SECTION_HDR(sym->st_shndx);
            if (sh->sh_type == SHT_PROGBITS 
                    || sh->sh_type == SHT_INIT_ARRAY 
                    || sh->sh_type == SHT_FINI_ARRAY) {
                if ((sh->sh_flags & SHF_ALLOC) == 0) {
                    c = '?';
                } else if (sh->sh_flags & SHF_EXECINSTR) {
                    c = 't';
                } else if (sh->sh_flags & SHF_WRITE) {
                    c = 'd';
                } else {
                    c = 'r';
                }
            } else if (sh->sh_type == SHT_NOBITS) {
                c = 'b';
            }
        } else if (sym->st_shndx == SHN_UNDEF) {
            c = 'u';
        }

        if (binding == STB_GLOBAL) {
            c = toupper(c);
        }
#if 0
        switch (type) {

        case STT_NOTYPE:
            c = 'U';
            break;
        case STT_OBJECT:
            if (sym->st_shndx == SHN_COMMON) {
                c = 'C';
            } else if (sym->st_shndx == SHN_ABS) {
                c = 'A';
            } else {
                Elf64_Shdr *sh = SECTION_HDR(sym->st_shndx);
                uint16_t str_index = sh->sh_name;
                const char* name = shstrtab+str_index;
                if (strcmp(name, ".rodata") == 0) {
                    c = 'R';
                } else if (strcmp(name, ".data") == 0) {
                    c = 'D';
                } else if (strcmp(name, ".bss") == 0) {
                    c = 'B';
                } else {
                    c = 't';
                }
            }
            break;
        case STT_FUNC:
            c = 'T';
            break;
        case STT_SECTION:
            c = ' ';
            break;
        case STT_FILE:
            c = ' ';
            break;
        default:
            c = '?';
            break;
        }

        if (binding == STB_LOCAL) {
            c = tolower(c);
        } else if (binding == STB_WEAK) {
            if (sym->st_shndx == SHN_UNDEF) {
                c = 'w';
            } else {
                c = 'W';
            }
        }
#endif

        buf_push(symbols, (Symbol){.address = sym->st_value, .type = c, .name = strtab + sym->st_name});
    }
    qsort(symbols, buf_len(symbols), sizeof(Symbol), name_compare);

    for (Symbol *s = symbols; s < buf_end(symbols); s++) {
        if (s->type != ' ' && *s->name) {
            if (s->type == 'u' || s->type == 'U') {
                printf("%16s %c %s\n", " ", s->type, s->name);
            } else {
                printf("%016lx %c %s\n", s->address, s->type, s->name);
            }
        }
    }
    return 0;
}
