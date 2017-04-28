#ifndef EFP_PARSER_H
#define EFP_PARSER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "lexer.h"
#include "tokens.h"

struct parser {
    char* prototype_file;
    struct element* prototype;
    struct lexer* lexer;
};

struct parser* parser_create(char* prototype_file);
struct element* parser_run(struct parser* parser, char* file_name);
void parser_free(struct parser* parser);

#endif
