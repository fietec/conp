#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#define CONP_IMPLEMENTATION
#include "conp.h"

uint64_t file_size(const char* file_path)
{
    struct stat file;
    if (stat(file_path, &file) == -1){
        return 0;
    }
    return (uint64_t) file.st_size;
}

// allocate and populate a string with the file's content
char* read_entire_file(char *file_path)
{
    if (file_path == NULL) return NULL;
    FILE *file = fopen(file_path, "r");
    if (file == NULL) return NULL;
    uint64_t size = file_size(file_path);
    char *content = (char*) calloc(size+1, sizeof(*content));
    if (!content){
        fclose(file);
        return NULL;
    }
    fread(content, 1, size, file);
    fclose(file);
    return content;
}
    
int main(void)
{
    char *filename = "testing.conf";
    char *content = read_entire_file(filename);
    assert(content != NULL);

    ConpLexer lexer = conp_init(content, strlen(content), filename);
    ConpEntry entry;
    while (conp_parse(&lexer, &entry)){
        conp_print_token(entry.key);
        printf(" - ");
        conp_print_token(entry.value);
        putchar('\n');
    }
    printf("-----\n");
    return 0;
}