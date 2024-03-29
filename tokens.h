
#ifndef EFP_TOKENS_H
#define EFP_TOKENS_H

#include <stdbool.h>
#include <stdlib.h>

enum token_type {
    // types
    TKN_EOF	= 0, TKN_ASSIGN, TKN_ERROR, TKN_COMMENT,
    TKN_STRING, TKN_NUMBER, TKN_IDENTIFIER,

    // operators
    TKN_OPEN_BRACKET, TKN_CLOSE_BRACKET,
    TKN_OPEN_SQUARE, TKN_CLOSE_SQUARE,
    TKN_OPEN_BRACE, TKN_CLOSE_BRACE, TKN_COMMA,
    TKN_COLON, TKN_EXCLAMATION, TKN_DIVIDER
};

struct token {
    enum token_type		type;
    uint32_t			line;
    uint32_t			column;
    uint32_t			position;
    uint32_t			start;
    uint32_t            end;
    uint32_t			fileid;
    bool				escaped;
};

#define NO_TOKEN				(struct token){0,0,0,0,0,0,0,0}

#endif
