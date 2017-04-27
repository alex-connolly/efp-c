

#ifndef EFP_AST_H
#define EFP_AST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tokens.h"

struct element {
    struct element* parent;
    char* key;
    int num_fields;
    struct field** fields;
    int num_elements;
    struct element** elements;
};

union field_value {
    int number;
    char* string;
    char* identifier;
};

struct field {
    char* key;
    union field_value* value;
    enum token_type type;
};

struct element* create_element(struct element* parent, char* key);
void create_field(struct element* parent, char* key, union field_value value, enum token_type t);

struct field* element_field(struct element* e, const char* key);
struct element* element_element(struct element* e, const char* key);

#endif
