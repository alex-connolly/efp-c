#include "parser.h"
#include <string.h>

void parse_statement(struct element* scope, struct parser* parser);
void parse_element_declaration(struct element* scope, struct parser* parser);
void parse_field_assignment(struct element* scope, struct parser* parser);
void prototype_parse_element(struct parser* parser, struct element* prototype);
void prototype_parse_field(struct parser* parser, struct element* prototype);
void prototype_parse_statement(struct parser* parser, struct element* prototype);
void parser_error(struct parser* parser, char* error);

#define LEXER_NEXT lexer_next(parser->lexer)
#define LEXER_NEXT_ENFORCE(type) struct token t = lexer_next(parser->lexer); if (t.type != type) {parser_error(parser, "Failed to enforce token\n"); }

void parser_error(struct parser* parser, char* error){

}



void prototype_parse_element(struct parser* parser, struct element* prototype){

    struct element* element = malloc(sizeof(struct element));
    // get the key
    struct token token = LEXER_NEXT;
    element->key = token_string(parser->lexer, token);

    // handle the opening brace
    LEXER_NEXT_ENFORCE(TKN_OPEN_BRACE)
    // move the scope to our new element and continue parsing
    prototype_parse_statement(parser, element);
}

void prototype_parse_array(struct parser* parser, struct field* field){

    struct field_value value;

    // possible array forms
    // [string], [2:string], [string:2], [2:string:2]

    // we have already parsed '['

    // first possible number
    struct token token  = LEXER_NEXT;
    switch(token.type){
        case TKN_NUMBER:
            value.min = token_int(lexer, token);

            LEXER_NEXT_ENFORCE(TKN_COLON);

            LEXER_NEXT_ENFORCE(TKN_IDENTIFIER);

            token = LEXER_NEXT
            switch(token.type){
                case TKN_COLON:
                    token = LEXER_NEXT_ENFORCE(TKN_NUMBER);
                    field.max = token.value.number;
                    return;
                case TKN_CLOSE_SQUARE:
                    return;
            }
            break;
        case TKN_IDENTIFIER:
            field->value->identifier = token.value.identifer;
            token = LEXER_NEXT
            switch(token.type){
                case TKN_COLON:
                    token = LEXER_NEXT_ENFORCE(TKN_NUMBER);
                    field->max = token.value.number;
                    LEXER_NEXT_ENFORCE(TKN_CLOSE_SQUARE);
                    return;
                case TKN_CLOSE_SQUARE:
                    return;
            }
            break;
    }
}

void prototype_parse_field(struct parser* parser, struct element* prototype){
    struct field* field = malloc(sizeof(struct element));
    // get the key
    struct token token = LEXER_NEXT;
    field->key = token_string(parser->lexer, token);

    LEXER_NEXT_ENFORCE(TKN_COLON);

    parse_type:

    token = LEXER_NEXT;
    switch(token.type){
        case TKN_IDENTIFIER:
            struct field_value value;
            value->value->string = token_string(parser->lexer, token)
            add_field_value(field, &value);
            break;
        case TKN_OPEN_SQUARE:
            prototype_parse_array(parser, field);
            break;
        default:
            break;
    }

    token = LEXER_NEXT;
    switch (token.type){
        case TKN_OPEN_BRACKET:
            token = LEXER_NEXT;
            switch(token.type){
                case TKN_NUMBER:
                    field->default->number = token_int(parser->lexer, token);
                    break;
                case TKN_IDENTIFIER:
                    field->default->identifer = token_string(parser->lexer, token);
                    break;
                case TKN_STRING:
                    field->default->string = token_string(parser->lexer, token);
                    break;
            }
            LEXER_NEXT_ENFORCE(TKN_CLOSE_BRACKET)
            break;
        case TKN_DIVIDER:
            goto parse_type;
            break;

}

void prototype_parse_statement(struct parser* parser, struct element* prototype){
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

void prototype_create(struct parser* parser){
    parser->prototype = malloc(sizeof(struct element));
    prototype_parse_statement(parser, parser->prototype);
}

void free_element(struct element* element){
    for (int i = 0; i < element->num_fields; i++){
        free(element->fields[i]);
    }
    for (int i = 0; i < element->num_elements; i++){
        free_element(element->elements[i]);
    }
    free(element);
}


void prototype_free(struct element* prototype){
    free_element(prototype);
}

struct parser* parser_create(char* prototype_file){
    FILE* p = fopen(prototype_file, "r");

    struct parser* p = malloc(sizeof(struct parser));
    struct lexer* l = lexer_create(src, len);
    p->offset = 0;
    p->lexer = l;
    prototype_create(parser);
    return p;
}

void parse_field_assignment(struct element* prototype, struct element* scope, struct parser* parser){
    struct field* field = malloc(sizeof(struct field));
    struct token token = parser->tokens[parser->offset++];

    char* key = malloc((token.end - token.start + 1)*(sizeof(char)));
    strncpy(key, parser->lexer->buffer + token.start, token.end - token.start);
    key[token.end - token.start] = '\0';

    printf("field (%d) has key %s\n", token.type, key);

    // process =
    parser->offset++;

    token = parser->tokens[parser->offset++];

    char* value = token_string(parser->lexer, token);

    printf("field extends from %d to %d\n", token.start, token.end);
    printf("field (%d) has value %s \n", token.type, value);

    union field_value val;
    val.identifier = value;
    create_field(scope, key, val, parser->tokens[parser->offset].type);

    if (parser->tokens[parser->offset].type == TKN_COMMA){
        parser->offset++;
    }
    parse_statement(scope, parser);
}

void parse_element_declaration(struct element* prototype, struct element* scope, struct parser* parser){

    struct token token = LEXER_NEXT;

    // process {
    parser->offset++;
    char* value = malloc((token.end - token.start + 1)*(sizeof(char)));
    strncpy(value, parser->lexer->buffer + token.start, token.end - token.start);
    value[token.end - token.start] = '\0';

    if (element_element(prototype, value) == NULL){
        parse_error("Line %d: \nInvalid key '%s' in element '%s'", parser->lexer->line, )
    } else {
        prototype = element_element(prototype, value);
    }

    printf("Parsing element declaration with key %s\n", value);

    struct element* element = create_element(scope, value);
    parse_statement(prototype, element, parser);
}

void parse_statement(struct element* prototype, struct element* scope, struct parser* parser){
    printf("Parsing statement with offset %d ...\n", parser->offset);
    if (parser->tokens[parser->offset].type == TKN_CLOSE_BRACE){
        parser->offset++;
        parse_statement(scope->parent, parser);
    }
    // we are interested in the second token in all .fire declarations
    // <key> {
    // <key> = <value>
    enum token_type type = parser->tokens[parser->offset + 1].type;
    switch (type){
        case TKN_ASSIGN:
            printf("Parsing field declaration...\n");
            parse_field_assignment(scope, parser);
            break;
        case TKN_OPEN_BRACE:
            printf("Parsing element...\n");
            parse_element_declaration(scope, parser);
            break;
        default:
            printf("what is this (%d)\n", parser->tokens[parser->offset + 1].type);
            printf("should be (%d)\n", TKN_OPEN_BRACE);
            break;
    }
}

struct element* parser_run(struct parser* parser){
    struct element* scope = malloc(sizeof(struct element));
    scope->fields = NULL;
    scope->elements = NULL;
    scope->num_fields = 0;
    scope->num_elements = 0;
    parse_statement(scope, parser);
    scope = element_element(scope, "fireVM");
    return scope;
}


void parser_free(struct parser* parser){
    free(parser);
}
