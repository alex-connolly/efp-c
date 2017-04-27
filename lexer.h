#ifndef FIREVM_LEXER_H
#define FIREVM_LEXER_H

#include <stdlib.h>
#include "tokens.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include "../utils.h"

// lexer adapted from the Gravity lexer, will rewrite when I have time

struct lexer {
    const char*	buffer;		// buffer
    uint32_t offset;			// current buffer offset (in bytes)
    uint32_t position;		// current buffer position (in characters)
    uint32_t length;			// buffer length (in bytes)
    uint32_t line;			// line counter
    uint32_t col;			// column counter

    struct token token;		    // current token
    bool peeking;		// flag to check if a peek operation is in progress
};

struct lexer* lexer_create(const char* src, size_t len);
void lexer_free(struct lexer* lexer);

#endif
