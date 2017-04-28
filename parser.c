#include "parser.h"

void parse_statement(struct parser* parser, struct element* prototype, struct element* scope);
void parse_element_declaration(struct parser* parser, struct element* prototype, struct element* scope);
void parse_field_assignment(struct parser* parser, struct element* prototype, struct element* scope);
void prototype_parse_element(struct parser* parser, struct element* prototype);
void prototype_parse_field(struct parser* parser, struct element* prototype);
void prototype_parse_statement(struct parser* parser, struct element* prototype);
void parser_error(struct parser* parser, char* error);

#define LEXER_NEXT lexer_next(parser->lexer)
#define LEXER_NEXT_ENFORCE(tkn) if ((token = lexer_next(parser->lexer)).type != tkn) {parser_error(parser, "Failed to enforce token\n"); }

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
            value.min = token_int(parser->lexer, token);

            LEXER_NEXT_ENFORCE(TKN_COLON);

            LEXER_NEXT_ENFORCE(TKN_IDENTIFIER);

            token = LEXER_NEXT;
            switch(token.type){
                case TKN_COLON:
                    LEXER_NEXT_ENFORCE(TKN_NUMBER);
                    value.max = token_int(parser->lexer, token);
                    return;
                case TKN_CLOSE_SQUARE:
                    return;
                default:
                    break;
            }
            break;
        case TKN_IDENTIFIER:
            value.value.identifier = token_string(parser->lexer, token);
            token = LEXER_NEXT;
            switch(token.type){
                case TKN_COLON:
                    token = LEXER_NEXT;
                    value.max = token_int(parser->lexer, token);
                    LEXER_NEXT_ENFORCE(TKN_CLOSE_SQUARE);
                    return;
                case TKN_CLOSE_SQUARE:
                    return;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    value.is_array = 1; // true
    add_field_value(field, value);
}

void prototype_parse_field(struct parser* parser, struct element* prototype){
    struct field* field = malloc(sizeof(struct element));
    // get the key
    struct token token = LEXER_NEXT;
    field->key = token_string(parser->lexer, token);

    LEXER_NEXT_ENFORCE(TKN_COLON);

    parse_type:

    struct field_value fvalue;

    token = LEXER_NEXT;
    switch(token.type){
        case TKN_IDENTIFIER:
            fvalue.value.string = token_string(parser->lexer, token);
            add_field_value(field, &fvalue);
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
                    fvalue.def.number = token_int(parser->lexer, token);
                    break;
                case TKN_IDENTIFIER:
                    fvalue.def.identifer = token_string(parser->lexer, token);
                    break;
                case TKN_STRING:
                    fvalue.def.string = token_string(parser->lexer, token);
                    break;
                default: break;
            }
            LEXER_NEXT_ENFORCE(TKN_CLOSE_BRACKET)
            break;
        case TKN_DIVIDER:
            goto parse_type;
            break;
        default: break;
    }
}

void prototype_parse_statement(struct parser* parser, struct element* prototype){

    if (lexer_peek(parser->lexer, 0) == TKN_CLOSE_BRACE){
        LEXER_NEXT;
        prototype_parse_statement(parser, prototype->parent);
    }
    switch(lexer_peek(parser->lexer, 1)){
        case TKN_OPEN_BRACE:
            prototype_parse_element(parser, prototype);
            break;
        case TKN_COLON:
            prototype_parse_field(parser, prototype);
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
        free(element->fields[i]->key);
        for (int j = 0; j < element->fields[i]->num_values; j++){
            free(element->fields[i]->values[j]);
        }
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
    struct parser* p = malloc(sizeof(struct parser));
    struct lexer* l = lexer_create(prototype_file);
    p->lexer = l;
    prototype_create(p);
    return p;
}

void parse_field_assignment(struct parser* parser, struct element* prototype, struct element* scope){
    struct field* field = malloc(sizeof(struct field));

    struct token token = LEXER_NEXT;

    char* key = token_string(parser->lexer, token);

    create_field(scope, key);

    printf("field (%d) has key %s\n", token.type, key);

    // process =
    LEXER_NEXT;

    token = LEXER_NEXT;

    char* value = token_string(parser->lexer, token);

    printf("field extends from %d to %d\n", token.start, token.end);
    printf("field (%d) has value %s \n", token.type, value);

    struct field_value val;
    val.value.identifier = value;

    add_field_value(field, &val);
    if (token.type == TKN_COMMA){
        LEXER_NEXT;
    }
    parse_statement(parser, prototype, scope);
}

void parse_element_declaration(struct parser* parser, struct element* prototype, struct element* scope){

    struct token token = LEXER_NEXT;

    char* key = token_string(lexer, token);

    // process {
    LEXER_NEXT;

    if (element_element(prototype, key) == NULL){
        //parse_error("Line %d: \nInvalid key '%s' in element '%s'", parser->lexer->line, )
    } else {
        prototype = element_element(prototype, key);
    }

    printf("Parsing element declaration with key %s\n", key);

    struct element* element = create_element(scope, key);
    parse_statement(parser, prototype, element);
}

void parse_statement(struct parser* parser, struct element* prototype, struct element* scope){
    struct token token = LEXER_NEXT;
    //printf("Parsing statement with offset %d ...\n", parser->offset);
    if (token.type == TKN_CLOSE_BRACE){
        LEXER_NEXT;
        parse_statement(parser, prototype->parent, scope->parent);
    }
    // we are interested in the second token in all .fire declarations
    // <key> {
    // <key> = <value>
    switch (token.type){
        case TKN_ASSIGN:
            printf("Parsing field declaration...\n");
            parse_field_assignment(parser, prototype, scope);
            break;
        case TKN_OPEN_BRACE:
            printf("Parsing element...\n");
            parse_element_declaration(parser, prototype, scope);
            break;
        default:
            //printf("what is this (%d)\n", parser->tokens[parser->offset + 1].type);
            printf("should be (%d)\n", TKN_OPEN_BRACE);
            break;
    }
}

struct element* parser_run(struct parser* parser, char* file_name){

    parser->lexer = lexer_create(file_name);
    struct element* scope = malloc(sizeof(struct element));
    scope->fields = NULL;
    scope->elements = NULL;
    scope->num_fields = 0;
    scope->num_elements = 0;
    parse_statement(parser, parser->prototype, scope);
    scope = element_element(scope, "fireVM");
    return scope;
}


void parser_free(struct parser* parser){
    free(parser);
}
