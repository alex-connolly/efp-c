

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

union value {
    int number;
    float decimal;
    char* string;
    char* identifier;
};

struct field_value {
    int is_array;               // should only be used in prototype tree
    enum token_type type;
    union value value;
    union value def;
    int min, max;                // for array sizes
};

struct field {
    char* key;
    int num_values;
    struct field_value** values;
};

struct element* create_element(struct element* parent, char* key);
void create_field(struct element* parent, char* key);
void add_field_value(struct field* field, struct field_value* value);

struct field* element_field(struct element* e, const char* key);
struct element* element_element(struct element* e, const char* key);

#endif
