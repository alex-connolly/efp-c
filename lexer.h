#ifndef FIREVM_LEXER_H
#define FIREVM_LEXER_H

#include <stdlib.h>
#include "tokens.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// lexer adapted from the Gravity lexer, will rewrite when I have time

struct lexer {
    const char*	buffer;
    uint32_t offset;			// current buffer offset (in bytes)
    uint32_t position;		// current buffer position (in characters)
    uint32_t length;			// buffer length (in bytes)
    uint32_t line;			// line counter
    uint32_t column;

    struct token token;		    // current token
};

enum token_type lexer_peek(struct lexer* lexer, int ahead);
struct token lexer_next(struct lexer* lexer);
struct lexer* lexer_create(const char* filename);
void lexer_free(struct lexer* lexer);

int token_int(struct lexer* lexer, struct token token);
char* token_string(struct lexer* lexer, struct token token);

#endif
