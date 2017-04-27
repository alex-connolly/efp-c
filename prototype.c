#include "prototype.h"


void prototype_parse_element(struct* parser parser, struct* element prototype){

    struct element* element = malloc(sizeof(struct element));
    // get the key
    LEXER_NEXT
    element->key = LEXER_TOKEN_VALUE

    // handle the opening brace
    LEXER_NEXT_ENFORCE('{')
    // move the scope to our new element and continue parsing
    parse_statement(parser, element);
}

void prototype_parse_field(struct* parser parser, struct* element prototype){
    struct field* field = malloc(sizeof(struct element));
    // get the key
    LEXER_NEXT
    field->key = LEXER_TOKEN_VALUE

    LEXER_NEXT_ENFORCE(':')

    LEXER_NEXT
    switch(LEXER_TOKEN_TYPE){
        case TKN_IDENTIFIER:
            field->value->identifer = LEXER_TOKEN_VALUE
            switch(LEXER_PEEK){
                case TKN_OPEN_BRACKET:
                    LEXER_NEXT;
                    break;
                default
            }
            break;
        case OPEN_BRACKET:

            break;
    }
    field->value->identifier =
}

void prototype_parse_statement(struct* parser parser, struct* element prototype){
    if (LEXER_PEEK == TKN_CLOSE_BRACE){
        LEXER_NEXT;
        parse_statement(lexer, prototype->parent);
    }
    switch(LEXER_DOUBLE_PEEK){
        case TKN_OPEN_BRACE:
            prototype_parse_element(lexer, prototype);
            break;
        case TKN_OPEN_BRACE:
            prototype_parse_field(lexer, prototype);
            break;
        default:
            return;
    }
}


void create_prototype(struct parser* parser){

    struct element* prototype = malloc(sizeof(struct element));
    prototype_parse_statement(parser, prototype);
    parser->prototype = prototype;
}
