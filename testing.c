#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#define CONP_IMPLEMENTATION
#include "conp.h"

#define return_defer(value) do{result = (value); goto defer;}while(0)
#define b2s(expr) ((expr)?"true":"false")

typedef struct{
    ConpToken key;
    ConpToken value;
} ConpPair;

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

bool parse(char *filename)
{
    bool result = true;
    char *content = read_entire_file(filename);
    assert(content != NULL && "Could not read file");
    ConpLexer lexer = conp_init(content, strlen(content), filename);
    ConpToken token;
    
    while (true){
        ConpPair pair;
        if (!conp_expect(&lexer, &token, CONP_VALUES)) return_defer(false);
        pair.key = token;
        if (!conp_expect(&lexer, &token, ConpToken_Sep)) return_defer(false);
        if (!conp_expect(&lexer, &token, CONP_VALUES)) return_defer(false);
        pair.value = token;
        conp_print_token(pair.key);
        printf(" - ");
        conp_print_token(pair.value);
        putchar('\n');
        conp_next(&lexer, &token);
        switch(token.type){
            case ConpToken_NewLine: continue;
            case ConpToken_End: return_defer(true);
            default:{
                fprintf(stderr, "[ERROR] "CONP_LOC_FMT" Expected [%s, %s], but got %s!\n", conp_loc_expand(token.loc), ConpTokenTypeNames[ConpToken_NewLine], ConpTokenTypeNames[ConpToken_End], ConpTokenTypeNames[token.type]);
                return_defer(false);
            }
        }
    }
  defer:
    free(content);
    return result;
}
    
int main(void)
{
    char *filename = "testing.conf";
    printf("%s: %s\n", filename, b2s(parse(filename)));
    return 0;
}