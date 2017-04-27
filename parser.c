#include "parser.h"
#include <string.h>

struct parser* parser_create(char* prototype, char* file){
    FILE* src = fopen()
    struct parser* p = malloc(sizeof(struct parser));
    struct lexer* l = lexer_create(src, len);
    p->offset = 0;
    p->lexer = l;
    size_t num_tokens;
    p->tokens = lexer_tokenize(l, &num_tokens);
    p->num_tokens = num_tokens;
    printf("Lexer found %u tokens!\n", p->num_tokens);
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

    char* value = malloc((token.end - token.start + 1)*(sizeof(char)));
    strncpy(value, parser->lexer->buffer + token.start, token.end - token.start);
    value[token.end - token.start] = '\0';


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

    struct token token = NEXT_TOKEN;

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
