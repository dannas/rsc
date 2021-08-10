#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

void* xmalloc(size_t num_bytes) {
    void *ptr = malloc(num_bytes);
    if (!ptr) {
        perror("malloc failed");
        exit(1);
    }
    return ptr;
}

size_t read_file(char **buf, const char *path) {
    *buf = NULL;
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *buf = xmalloc(len + 1);

    if (len && fread(*buf, len, 1, fp) != 1) {
        fclose(fp);
        free(*buf);
        return -1;
    }
    (*buf)[len] = '\0';
    fclose(fp);
    return len;
}

int main(int argc, char *argv[]) {
    char *buf = NULL;
    const char *path = "size";
    size_t len = read_file(&buf, path);

#define SECTION_HDR(index) ((Elf64_Shdr*)(buf+ehdr->e_shoff + (index) * ehdr->e_shentsize))
#define PROGRAM_HDR(index) ((Elf64_Phdr*)(buf+ehdr->e_phoff + (index) * ehdr->e_phentsize))

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

    int bss = 0;
    int text = 0;
    int data = 0;


    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr *phdr = PROGRAM_HDR(i);
        if (phdr->p_type == PT_LOAD) {
            if (phdr->p_flags & PF_X) {
                //text += phdr->p_memsz;
                ;
            } else {
                //data += phdr->p_memsz;
                ;
            }
        }
    }
    // Find the string table, .shstrtab, containing the strings for the section names.
    char *shstrtab = buf + SECTION_HDR(ehdr->e_shstrndx)->sh_offset;

    Elf64_Shdr *symtab_hdr = NULL;
    char *strtab = NULL;
    int type;


    for (int i = 0 ; i < ehdr->e_shnum; i++) {
        Elf64_Shdr *sh = SECTION_HDR(i);
        const char* name = shstrtab + sh->sh_name;
        if ((sh->sh_flags & SHF_ALLOC)) {
            if ((sh->sh_flags & SHF_EXECINSTR) && !(sh->sh_flags & SHF_WRITE)) {
                type = 't';
                text += sh->sh_size;
            } else if (sh->sh_type != SHT_NOBITS) {
                type = 'd';
                data += sh->sh_size;
            }
        } else if (sh->sh_type == SHT_NOBITS) {
            type = 'b';
            bss += sh->sh_size;
        }
        printf("%20s %10d %10c\n", name, sh->sh_size, type);
    }
    printf("bss=%d data=%d text=%d\n", bss, data, text);
    return 0;
}
