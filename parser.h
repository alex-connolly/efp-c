#ifndef EFP_PARSER_H
#define EFP_PARSER_H

#include <stdlib.h>
#include "ast.h"
#include "lexer.h"
#include <stdio.h>

struct parser {
    char* prototype_file;
    struct element* prototype;
    struct lexer* lexer;
};

struct parser* parser_create(char* prototype);
struct element* parser_run(struct parser* parser, char* file);
void parser_free(struct parser* parser);

void parse_statement(struct element* scope, struct parser* parser);
void parse_element_declaration(struct element* scope, struct parser* parser);
void parse_field_assignment(struct element* scope, struct parser* parser);

#endif
