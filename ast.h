

#ifndef EFP_AST_H
#define EFP_AST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tokens.h"
#include "util.h"

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
    float decimal;
    char* string;
    char* identifier;
};

struct field {
    char* key;
    union field_value* value;
    enum token_type type;
    int max, min;                   // for array sizes
};

struct element* create_element(struct element* parent, char* key);
void create_field(struct element* parent, char* key, enum token_type t, union field_value* value);

struct field* element_field(struct element* e, const char* key);
struct element* element_element(struct element* e, const char* key);

#endif
